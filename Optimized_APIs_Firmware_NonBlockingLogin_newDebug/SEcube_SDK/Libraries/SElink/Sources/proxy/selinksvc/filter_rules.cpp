/**
 *  \file filter_rules.cpp
 *  \author Nicola Ferri
 *  \brief Filter rules management functions
 */

#include "selinksvc/filter_rules.h"

#include <locale>
#include <algorithm>
#include <stdexcept>

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>

#include <boost/algorithm/string.hpp>
#include <boost/range/adaptor/transformed.hpp>
#include <boost/format.hpp>
#include <boost/asio.hpp>
#include <boost/log/trivial.hpp>

#include "util/filesystem.h"

namespace selink { namespace service { namespace wfp {

    std::string filter_condition::str()
    {
        std::stringstream ss;
        if (!exe_path.empty())
            ss << "exe=\"" << exe_path << "\" ";
        if (port_beg >= 0)
            ss << "ports=" << port_beg << "-" << port_end << " ";
        if (!address.empty())
            ss << "addr=" << filter_rule_table::address_str(address) << " ";
        return ss.str();
    }

    std::string filter_action::str()
    {
        switch (action) {
        case allow:
            return "allow";
        case block:
            return "block";
        case encrypt:
        {
            std::stringstream ss;
            ss << "encrypt:" << key_id;
            return ss.str();
        }
        default:
            return "invalid";
        }
    }

    std::string filter_entity::str()
    {
        std::stringstream ss;
        ss << "exe=" << exe_path <<
            " addr=" << filter_rule_table::address_str(address) <<
            " port=" << port;
        return ss.str();
    }

    filter_rule_table::filter_rule_table()
    {
        default_act.action = filter_action::allow;
        default_act.key_id = 0;
    }

    filter_rule_table::~filter_rule_table()
    { }

    void filter_rule_table::default_action(filter_action default_act_)
    {
        boost::mutex::scoped_lock lock1(mut);

        default_act = default_act_;
    }

    void filter_rule_table::update(std::list<filter_rule> newrules)
    {
        boost::mutex::scoped_lock lock1(mut);

        rules.clear();
        rules.reserve(newrules.size());
        size_t i = 0;
        for (auto rule : newrules) {
            if (!rule.cond.address.empty() &&
                rule.cond.address.size() != 4 &&
                rule.cond.address.size() != 16)
            {
                continue;
            }

            filter_rule_hashed rh;
            rh.act = rule.act;
            rh.cond = rule.cond;
            rh.exe_any = rh.cond.exe_path.empty();
            if (!rh.exe_any)
                rh.exe_hash = hash_string(rh.cond.exe_path);
            rh.address_any = rh.cond.address.empty();
            if (!rh.address_any)
                rh.address_hash = hash_address(rh.cond.address);
            rh.port_any = (rh.cond.port_beg < 0);

            BOOST_LOG_TRIVIAL(trace) <<
                "RULE #" << i << " "  << rh.cond.str() << " ACTION " << rh.act.str();
            rules.push_back(rh);
            i++;
        }
        BOOST_LOG_TRIVIAL(trace) << "Read " << rules.size() << " rules";
    }

    std::string filter_rule_table::address_str(std::vector<uint8_t> address)
    {
        if (address.size() == 4) {
            // ".".join(map(lambda x:"%u"%x, address))
            return boost::algorithm::join(address |
                boost::adaptors::transformed([](uint8_t x) {
                return (boost::format("%u") % ((unsigned)x)).str();
            }), ".");
        }
        else if (address.size() == 16) {
            std::vector<uint16_t> tmp(8);
            memcpy(&tmp[0], &address[0], address.size());
            // ":".join(map(lambda x:"%02x%02x"%(x.high, x.low), tmp))
            return boost::algorithm::join(
                tmp | boost::adaptors::transformed(
                    [](uint16_t x) {
                        return (boost::format("%02x%02x") % (x & 0xFF) % ((x >> 8) & 0xFF)).str();
                    }),
                ":");
        }
        return "";
    }

    filter_action filter_rule_table::match(filter_entity m)
    {
        std::size_t exe_h = hash_string(m.exe_path);
        std::size_t address_h = hash_address(m.address);

        boost::mutex::scoped_lock lock1(mut);
        BOOST_LOG_TRIVIAL(trace) << m.str();

        int i = -1;
        for (auto rule : rules) {
            i++;
            // check if rule matches
            if (!rule.exe_any && rule.exe_hash != exe_h) continue;
            if (!rule.address_any && rule.address_hash != address_h) continue;
            if (!rule.port_any &&
                (m.port < rule.cond.port_beg || m.port > rule.cond.port_end)) continue;
            if (!rule.address_any && rule.cond.address != m.address) continue;
            if (!rule.exe_any && rule.cond.exe_path != m.exe_path) continue;

            BOOST_LOG_TRIVIAL(trace) << "FILTER hit #" << i << " ACTION " << rule.act.str();
            return rule.act;
        }
        BOOST_LOG_TRIVIAL(trace) << "FILTER miss ACTION " << default_act.str();
        return default_act;
    }


    bool filter_rule_table::update_from_file(boost::filesystem::path config_path)
    {
        try {
            boost::filesystem::ifstream file(config_path);
            file.imbue(std::locale());

            boost::property_tree::ptree pt;
            boost::property_tree::read_json(file, pt);

            auto parse_action = [](std::string& s) -> int {
                if (s == "allow")return filter_action::allow;
                if (s == "block")return filter_action::block;
                if (s == "encrypt")return filter_action::encrypt;
                return -1;
            };

            std::list<filter_rule> newrules;

            // property names map for switch(property_name) construct
            enum pn : int {
                invalid, description, path, address,
                port_first, port_last, action, key_id };
            std::unordered_map<std::string, int> pnmap_ = {
                { "description", pn::description }, { "path", pn::path },
                { "address", pn::address }, { "port-first", pn::port_first },
                { "port-last", pn::port_last }, { "action", pn::action },
                { "key-id", pn::key_id }};
            auto pnmap = [&](std::string const& key) -> int {
                auto res = pnmap_.find(key);
                return (res == pnmap_.end()) ? (pn::invalid) : (res->second);
            };

            for (auto& prop : pt) {
                if (prop.first != "rules") {
                    throw std::runtime_error("invalid property found in root");
                }

                for (auto& li : prop.second) {
                    filter_rule rule;
                    for (auto rule_prop : li.second) {
                        switch (pnmap(rule_prop.first)) {
                        case pn::description:
                            break;
                        case pn::path:
                        {
                            boost::filesystem::path exe_path(
                                rule_prop.second.get_value<std::string>());
                            // slash -> backslash
                            rule.cond.exe_path =
                                selink::util::filesystem::path_uniform_case(
                                    exe_path.make_preferred());
                            break;
                        }

                        case pn::address:
                        {
                            boost::asio::ip::address asio_address =
                                boost::asio::ip::address::from_string(
                                    rule_prop.second.get_value<std::string>());
                            std::vector<uint8_t> address;
                            if (asio_address.is_v4()) {
                                address.resize(4);
                                std::array<uint8_t, 4> tmp =
                                    asio_address.to_v4().to_bytes();
                                std::copy(tmp.begin(), tmp.end(), address.begin());
                            }
                            else if (asio_address.is_v6()) {
                                address.resize(16);
                                std::array<uint8_t, 16> tmp =
                                    asio_address.to_v6().to_bytes();
                                std::copy(tmp.begin(), tmp.end(), address.begin());
                            }
                            else {
                                throw std::runtime_error("invalid address found");
                            }
                            rule.cond.address = address;
                            break;
                        }
                        case pn::port_first:
                            rule.cond.port_beg = rule_prop.second.get_value<int>();
                            break;
                        case pn::port_last:
                            rule.cond.port_end = rule_prop.second.get_value<int>();
                            break;
                        case pn::action:
                        {
                            int action = parse_action(
                                rule_prop.second.get_value<std::string>());
                            if (action == -1) {
                                throw std::runtime_error("invalid action found");
                            }
                            rule.act.action = action;
                            break;
                        }
                        case pn::key_id:
                            rule.act.key_id = rule_prop.second.get_value<int>();
                            break;
                        default:
                            throw std::runtime_error("invalid property found in rule");
                        }
                    }
                    newrules.push_back(rule);
                }
            }
            update(newrules);
            return true;
        }
        catch (std::exception const& e) {
            BOOST_LOG_TRIVIAL(error) <<
                "Error parsing filter rules: " << e.what();
        }
        return false;
    }

} } }  // namespace selink::service::wfp

/**
 *  \file portmap_acceptor.cpp
 *  \author Nicola Ferri
 *  \brief Acceptor for SElink gateway
 */

#include "selinkgw/portmap_acceptor.h"

#include <sstream>
#include <locale>
#include <list>
#include <stdexcept>

#include <boost/log/trivial.hpp>
#include <boost/bind.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>

namespace selink { namespace service { namespace portmap {

    using boost::asio::ip::tcp;

    acceptor::acceptor(boost::asio::io_service& io, selink::provider::provider& ep)
        : ep_(ep), io_(io), strand_encrypt(io)
    { }
    
    acceptor::~acceptor()
    {
        portconfig conf;
        update(conf);
    }


    bool acceptor::update_from_file(boost::filesystem::path config_path)
    {
        try {
            boost::filesystem::ifstream file(config_path);
            file.imbue(std::locale());

            boost::property_tree::ptree pt;
            boost::property_tree::read_json(file, pt);
            portconfig conf;

            for (auto& prop : pt) {
                if (prop.first != "map") {
                    throw std::runtime_error("invalid property found in root");
                }
                for (auto& li : prop.second) {
                    // property name map for switch(property_name) construct
                    enum pn : int {
                        invalid, description, listen_port,
                        redirect_host, redirect_port, key_id };
                    std::unordered_map<std::string, int> pnmap_ = {
                        { "description", pn::description },
                        { "listen-port", pn::listen_port },
                        { "redirect-host", pn::redirect_host },
                        { "redirect-port", pn::redirect_port },
                        { "key-id", pn::key_id }};
                    auto pnmap = [&](std::string const& key) -> int {
                        auto res = pnmap_.find(key);
                        return (res == pnmap_.end()) ? (pn::invalid) : (res->second);
                    };

                    config_entry ce = { "", 0, -1 };
                    uint16_t port = 0;
                    for (auto& map_prop : li.second) {
                        switch (pnmap(map_prop.first)) {
                        case pn::description:
                            break;
                        case pn::listen_port:
                            port = map_prop.second.get_value<uint16_t>();
                            break;
                        case pn::redirect_host:
                        {
                            boost::asio::ip::address address =
                                boost::asio::ip::address::from_string(
                                    map_prop.second.get_value<std::string>());
                            ce.redirect_host = address.to_string();
                            break;
                        }
                        case pn::redirect_port:
                            ce.redirect_port = map_prop.second.get_value<uint16_t>();
                            break;
                        case pn::key_id:
                            ce.key_id = map_prop.second.get_value<int>();
                            break;
                        default:
                            throw std::runtime_error("invalid property found in map entry");
                        }
                    }
                    if (port == 0 || ce.redirect_host.empty() ||
                        ce.redirect_port == 0 || ce.key_id == -1)
                    {
                        throw std::runtime_error("invalid values found in map entry");
                    }
                    conf[port] = ce;
                }
            }
            update(conf);
            return true;
        }
        catch (std::exception const& e) {
            BOOST_LOG_TRIVIAL(error) <<
                "Error parsing port map: " << e.what();
        }
        return false;
    }

    std::string acceptor::config_entry::str()
    {
        std::stringstream ss;
        ss << redirect_host << " port " << (unsigned)redirect_port << " key " << key_id;
        return ss.str();
    }

    void acceptor::stop() {
        portconfig conf;
        update(conf);
    }


    void acceptor::update(portconfig conf)
    {
        boost::mutex::scoped_lock lock1(mut);

        for (auto rule : conf) {
            BOOST_LOG_TRIVIAL(trace) <<
                "RULE " << rule.first << " => " << rule.second.str();
        }

        // remove accepptors that are not in new conf or have changed
        std::list<uint16_t> toremove;
        for (auto& ac : acceptors) {
            auto e = conf.find(ac.first);
            if (e == conf.end()) {
                toremove.push_back(ac.first);
            }
            else {
                if (e->second.redirect_host != ac.second.conf.redirect_host ||
                    e->second.redirect_port != ac.second.conf.redirect_port)
                {
                    toremove.push_back(ac.first);
                }
            }
        }

        for (uint16_t port : toremove) {
            auto ac = acceptors.find(port);
            if (ac != acceptors.end()) {
                if (ac->second.acceptor_v4 != nullptr) {
                    ac->second.acceptor_v4->close();
                    delete ac->second.acceptor_v4;
                    ac->second.acceptor_v4 = nullptr;
                }
                if (ac->second.acceptor_v6 != nullptr) {
                    ac->second.acceptor_v6->close();
                    delete ac->second.acceptor_v6;
                    ac->second.acceptor_v6 = nullptr;
                }
                acceptors.erase(port);
            }
        }

        // add new acceptors
        for (auto e : conf) {
            auto ac = acceptors.find(e.first);
            if (ac == acceptors.end()) {
                auto result = acceptors.insert({ e.first, table_entry() });
                if (result.second) {
                    table_entry& entry = result.first->second;
                    entry.conf = e.second;
                    entry.acceptor_v4 = nullptr;
#ifdef _WIN32
                    try {
                        entry.acceptor_v4 = new tcp::acceptor(
                            io_, tcp::endpoint(boost::asio::ip::address_v4::any(), e.first));
                    }
                    catch (boost::system::system_error& e) {
                        BOOST_LOG_TRIVIAL(error) << "bind(v4) failed: " << e.what();
                        entry.acceptor_v4 = nullptr;
                    }
#endif  // _WIN32
                    try {
                        entry.acceptor_v6 = new tcp::acceptor(
                            io_, tcp::endpoint(boost::asio::ip::address_v6::any(), e.first));
                    }
                    catch (boost::system::system_error& e) {
                        BOOST_LOG_TRIVIAL(error) << "bind(v6) failed: " << e.what();
                        entry.acceptor_v6 = nullptr;
                    }

                    if (entry.acceptor_v4 != nullptr) {
                        start_accept(entry.acceptor_v4, entry.conf);
                    }
                    if (entry.acceptor_v6 != nullptr) {
                        start_accept(entry.acceptor_v6, entry.conf);
                    }
                }
            }
        }
    }

    bool acceptor::start_accept(
        tcp::acceptor* acceptor,
        config_entry c)
    {
        try {
            proxy::filter::pointer flt =
                selink::proxy::filter::create(acceptor->get_io_service(), strand_encrypt, ep_);

            acceptor->async_accept(
                flt->get_fb(),
                boost::bind(
                    &acceptor::handle_accept,
                    this,
                    flt,
                    acceptor,
                    c,
                    boost::asio::placeholders::error));
        }
        catch (std::exception&) {
            return false;
        }
        return true;
    }

    void acceptor::handle_accept(
        proxy::filter::pointer flt,
        tcp::acceptor* acceptor,
        config_entry c,
        const boost::system::error_code& error)
    {
        if (!error) {
            flt->set_redirect_info(
                c.redirect_host,
                c.redirect_port,
                proxy::filter::side_accept,
                true,
                c.key_id);
            flt->start();

            BOOST_LOG_TRIVIAL(trace) << "CONNECT MAP " <<
                flt->get_fb().remote_endpoint().address().to_string() << ":" <<
                (unsigned)flt->get_fb().remote_endpoint().port() <<
                " -> " << c.redirect_host << ":" << (unsigned)c.redirect_port;
            start_accept(acceptor, c);
        }
        else {
            BOOST_LOG_TRIVIAL(trace) << "handle_accept error";
        }
    }


} } }


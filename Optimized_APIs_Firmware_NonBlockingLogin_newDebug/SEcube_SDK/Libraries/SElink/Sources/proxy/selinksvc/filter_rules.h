/**
 *  \file filter_rules.h
 *  \author Nicola Ferri
 *  \brief Filter rules management functions
 */

#pragma once

#include "util/os.h"
#include "util/os_tcpip.h"

#include <cstdint>
#include <vector>
#include <string>
#include <list>
#include <unordered_map>

#include <boost/thread.hpp>
#include <boost/filesystem.hpp>
#include <boost/functional/hash.hpp>

namespace selink { namespace service { namespace wfp {

    /** Conditions for a connection to match a filter rule */
    struct filter_condition {
        /** Executable path. If empty, anything matches */
        std::string exe_path;
        /** IP address. If empty, anything matches */
        std::vector<uint8_t> address;
        /** @{
         *  Port range. if both -1, anything matches.
         */
        int port_beg{ -1 };
        int port_end{ -1 };
        /** @} */
        std::string str();
    };

    /** Type of action that can be taken with a new connection */
    struct filter_action {
        enum {
            allow = 0,
            block = 1,
            encrypt = 2
        };
        int key_id{ -1 };
        int action{ 0 };
        std::string str();
    };

    /** A filter rule, consisting of a condition and an action
     */
    struct filter_rule {
        filter_condition cond;
        filter_action act;
    };

    /** An object which can be compared with a filter rule.
     */
    class filter_entity {
    public:
        std::string exe_path;
        std::vector<uint8_t> address;
        int port;
        std::string str();
    };

    /** An ordered list of filter rules.
     */
    class filter_rule_table {
    public:
        filter_rule_table();
        ~filter_rule_table();

        /** \brief Set the default action
         *  \param default_act_ a filter action
         *  
         *  Set the action that match() will return when no rule in the list matches 
         */
        void default_action(filter_action default_act_);

        /** \brief Update filter rules
         *  \param newrules List of filter rules
         *  
         *  Discard all rules and store the provided list of rules
         */
        void update(std::list<filter_rule> newrules);

        /** \brief Update filter rules from file
         *  \param config_path path of the filter rules file
         *  
         *  Read new rules from the provided JSON file and store them,
         *  replacing any previously stored rule
         */
        bool update_from_file(boost::filesystem::path config_path);

        /** \brief Find matching rule
         *  \param m a filter entity object that can be compared with filter rules
         *  \return the action associated with the matching rule
         *  
         *  Find a matching rule in the filter rule table. The list is scanned in order
         *  and the first matching rule is picked.
         */
        filter_action match(filter_entity m);

        /** \brief Convert IP address to string
         *  
         *  For logging purposes
         */
        static std::string address_str(std::vector<uint8_t> address);

    private:
        /** A filter rule with pre-computed hashes of string and vector fields,
         *  for faster matching.
         */
        struct filter_rule_hashed {
            filter_condition cond;
            filter_action act;
            bool exe_any{ true };
            std::size_t exe_hash{ 0 };
            bool address_any{ true };
            std::size_t address_hash{ 0 };
            bool port_any{ true };
        };

        /** The actual filter rule list */
        std::vector<filter_rule_hashed> rules;

        /** Default action to be returned when no rule marches */
        filter_action default_act;

        boost::hash<std::string> hash_string;
        boost::hash<std::vector<uint8_t>> hash_address;
        boost::mutex mut;
    };

} } }  // namespace selink::service::wfp

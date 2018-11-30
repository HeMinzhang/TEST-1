/**
 *  \file portmap_acceptor.h
 *  \author Nicola Ferri
 *  \brief Acceptor for SElink gateway
 *
 *  Accept connections and set up the filter component with the necessary information for
 *    bridging.
 */

#pragma once
#include "util/os.h"

#include <string>
#include <unordered_map>
#include <memory>

#include <boost/thread.hpp>
#include <boost/filesystem.hpp>
#include <boost/filesystem/fstream.hpp>
#include <boost/asio.hpp>

#include "proxy/provider/provider.h"
#include "proxy/filter.h"
#include "util/filesystem.h"

namespace selink { namespace service { namespace portmap {

    using boost::asio::ip::tcp;

    class acceptor {
    public:
        // (accept_port, (redirect_host, redirect_port, key_id))
        struct config_entry {
            std::string redirect_host;
            uint16_t redirect_port;
            int key_id;
            std::string str();
        };
        typedef std::unordered_map<uint16_t, config_entry> portconfig;

        acceptor(boost::asio::io_service& io, selink::provider::provider& ep);
        ~acceptor();
        void update(portconfig conf);
        bool update_from_file(boost::filesystem::path config_path);
        void stop();
        bool start_accept(
            tcp::acceptor* acceptor,
            config_entry conf);
        void handle_accept(
            proxy::filter::pointer flt,
            tcp::acceptor* acceptor,
            config_entry c,
            const boost::system::error_code& error);
        
    private:
        struct table_entry {
            config_entry conf;
            tcp::acceptor* acceptor_v4{ nullptr };
            tcp::acceptor* acceptor_v6{ nullptr };
        };
        typedef std::unordered_map<uint16_t, table_entry> table;
        table acceptors;
        selink::provider::provider& ep_;
        boost::mutex mut;
        boost::asio::io_service& io_;
        boost::asio::strand strand_encrypt;
    };

} } }




/**
 *  \file selinkgw.h
 *  \author Nicola Ferri
 *  \brief SElink gateway global structures and initialization
 */

#pragma once

#include "util/os.h"

#include <cstdint>
#include <memory>
#include <vector>

#include <boost/asio.hpp>
#include <boost/filesystem.hpp>

#include "proxy/provider/provider.h"
#include "selinkgw/portmap_acceptor.h"

namespace selink { namespace service { namespace portmap {

    /** Global variables for the service */
    struct globals_ {
        bool is_service{ false };

        boost::filesystem::path config_path{ "selinkgw.json" };
        boost::filesystem::path key_file_path{ "" };

        selink::provider::provider_type encryption_provider_type{
            selink::provider::provider_type::soft
        };
        std::unique_ptr<selink::provider::provider> encryption_provider{ nullptr };
        std::unique_ptr<selink::service::portmap::acceptor> proxy_server{ nullptr };

        std::vector<uint8_t> device_pin;
        std::array<uint8_t, selink::provider::provider::sn_size> device_sn;

        boost::asio::io_service io;
    };

    bool initialize_app();
    extern globals_ globals;
    void network_main();
    void proxy_server_update();

} } }  // namespace selink::service::portmap


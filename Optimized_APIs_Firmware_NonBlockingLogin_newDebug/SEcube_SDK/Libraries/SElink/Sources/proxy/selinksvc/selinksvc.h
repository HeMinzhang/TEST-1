/**
 *  \file selinksvc.h
 *  \author Nicola Ferri
 *  \brief SElink service global structures and initialization
 */

#pragma once

#include "util/os.h"

#include <fwpmu.h>
#include <guiddef.h>

#include <cstdint>
#include <string>
#include <memory>

#include <boost/asio.hpp>

#include "proxy/provider/provider.h"

#include "selinksvc/wfp_acceptor.h"
#include "selinksvc/ipc_server.h"
#include "selinksvc/filter_rules.h"

namespace selink { namespace service { namespace wfp {

    /** Global variables for the service */
    struct globals_ {
        // service
        const std::wstring service_name{ L"selinksvc" };
        bool is_service{ false };
        SERVICE_STATUS service_status{ { 0 } };
        SERVICE_STATUS_HANDLE service_status_handle{ NULL };

        // windows filtering platform
        uint16_t wfp_port{ 10200 };

        // boost.asio io service
        boost::asio::io_service io;
        std::unique_ptr<wfp::acceptor> proxy_server{ nullptr };

        // encryption provider
        selink::provider::provider_type encryption_provider_type{
            selink::provider::provider_type::secube
        };
        std::unique_ptr<selink::provider::provider> encryption_provider{ nullptr };
        boost::filesystem::path key_file_path{ "" };

        // filter rules file
        boost::filesystem::path config_path{ "selinksvc.json" };
    };
    extern globals_ globals;

    /** Initialize the application from the supplied parameters in the global structure
     *  Depending on the value contained in globals.encryption_provider_type, a suitable provider
     *  is initialized. globals.key_file_path may also be needed for some providers.
     *  Filter rules are loaded from globals.config_path
     */
    bool initialize_app();

    /** Main network thread entry point
     *  - Initializes the acceptor
     *  - Initializes the IPC server
     *  - Processes the boost.asio io_service queue
     *  When the io_service is stopped, the thread exits. Additional threads may be created
     *  for concurrency, but are guarenteed to be joined before the main thread exits.
     */
    void network_main();

    /** \brief Update filter rules from the config file
     *  \param proxy_server the acceptor object in use
     */
    void rules_update(selink::service::wfp::acceptor& proxy_server);

} } }  // namespace selink::service::wfp






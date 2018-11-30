/**
 *  \file selinkgw.cpp
 *  \author Nicola Ferri
 *  \brief SElink gateway global structures and initialization
 */

#include "selinkgw/selinkgw.h"

#include <boost/log/trivial.hpp>

#include "proxy/provider/provider_soft.h"
#include "proxy/provider/provider_secube.h"

namespace selink { namespace service { namespace portmap {

    globals_ globals;

    /** @{
     *  Encryption provider initialization
     */
    static bool initialize_provider();
    static bool initialize_provider_soft();
    static bool initialize_provider_secube();
    /** @} */

    bool initialize_app()
    {
        if (!initialize_provider()) {
            BOOST_LOG_TRIVIAL(fatal) << "Error initializing the encryption provider";
            return false;
        }
        return true;
    }

    void network_main()
    {
        globals.proxy_server =
            std::unique_ptr<selink::service::portmap::acceptor>(
                new selink::service::portmap::acceptor(globals.io, *globals.encryption_provider));

        // load port mappings
        proxy_server_update();

        globals.io.run();
    }

    void proxy_server_update() {
        if (!globals.proxy_server->update_from_file(globals.config_path))
            BOOST_LOG_TRIVIAL(error) << "Could not read config file";
    }


    // ---- static ----

    static bool initialize_provider()
    {
        switch (globals.encryption_provider_type)
        {
        case selink::provider::provider_type::soft:
            return initialize_provider_soft();
        case selink::provider::provider_type::secube:
            return initialize_provider_secube();
        }
        return false;
    }

    static bool initialize_provider_soft()
    {
        BOOST_LOG_TRIVIAL(trace) << "Provider: soft";
        if (globals.key_file_path.empty()) {
            BOOST_LOG_TRIVIAL(fatal) << "Error reading keys from key file";
            return false;
        }
        auto keys = selink::provider::provider_soft::keys_from_file(globals.key_file_path);
        if (keys.empty()) {
            BOOST_LOG_TRIVIAL(fatal) << "Error reading keys from key file";
            return false;
        }
        BOOST_LOG_TRIVIAL(trace) << "Read " << (unsigned)keys.size() << " keys";
        selink::provider::provider_soft* ep = new selink::provider::provider_soft();
        ep->set_keys(keys);
        globals.encryption_provider = std::unique_ptr<selink::provider::provider>(ep);
        return true;
    }

    static bool initialize_provider_secube()
    {
        BOOST_LOG_TRIVIAL(trace) << "Provider: secube";
        selink::provider::provider_secube* ep = new selink::provider::provider_secube();
        globals.encryption_provider = std::unique_ptr<selink::provider::provider>(ep);
        ep->config(globals.device_sn, globals.device_pin);
        ep->login();
        return true;
    }

} } }  // namespace selink::service::portmap

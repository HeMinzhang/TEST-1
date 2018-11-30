/**
 *  \file selinksvc.cpp
 *  \author Nicola Ferri
 *  \brief SElink service global structures and initialization
 */

#include "selinksvc/selinksvc.h"

#include <boost/scoped_ptr.hpp>
#include <boost/log/trivial.hpp>

#include "proxy/provider/provider_soft.h"
#include "proxy/provider/provider_secube.h"

namespace selink { namespace service { namespace wfp {

    globals_ globals;

    /** @{
    *  Encryption provider initialization
    */
    static bool initialize_provider();
    static bool initialize_provider_soft();
    static bool initialize_provider_secube();
    /** @} */

    // entry point for additional io service thread
    static void network_io_runner(boost::asio::io_service& io);

    bool initialize_app()
    {
        // make current PID available to the driver
        wfp::helper::add_pid_to_wfp_context(GetCurrentProcessId());

        if (!initialize_provider()) {
            BOOST_LOG_TRIVIAL(fatal) << "Error initializing the encryption provider";
            return false;
        }
        return true;
    }

    bool initialize_provider()
    {
        switch (globals.encryption_provider_type)
        {
        case selink::provider::provider_type::soft:
            return initialize_provider_soft();
        case selink::provider::provider_type::secube:
            return initialize_provider_secube();
        //case selink::provider::provider_type::se3:
        //    return initialize_provider_se3();
        }
        return false;
    }

    void network_main()
    {
        // instantiate and configure proxy object
        wfp::acceptor proxy_server(
            globals.io, 10200, *globals.encryption_provider);
        rules_update(proxy_server);
        
        // instantiate IPC object and run IPC io on separate thread
        boost::scoped_ptr<wfp::ipc> ipc_listener;
        try{
            ipc_listener.reset(new wfp::ipc());
        }
        catch(std::exception const&){
            BOOST_LOG_TRIVIAL(fatal) << "Could not start the IPC server";
            return;
        }
        boost::thread ipc_thread(
            ipc_main, boost::ref(*ipc_listener), boost::ref(proxy_server));

        // start accepting connections on the io service
        proxy_server.start_accept();
        
        // run additional threads for the io service
        boost::thread io2_thread(
            network_io_runner, boost::ref(globals.io));

        // process io queue until stopped
        globals.io.run();
        // io service stopped

        ipc_listener->stop();  // signal the IPC server to stop

        io2_thread.join();  // join additional network threads
        ipc_thread.join();  // join IPC server thread
    }

    void rules_update(selink::service::wfp::acceptor& proxy_server)
    {
        if (!proxy_server.rules.update_from_file(globals.config_path))
            BOOST_LOG_TRIVIAL(error) << "Could not read config file";
    }


    // ---- static ----

    static void network_io_runner(boost::asio::io_service& io)
    {
        io.run();
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
        return true;
    }

} } }  // namespace selink::service::wfp

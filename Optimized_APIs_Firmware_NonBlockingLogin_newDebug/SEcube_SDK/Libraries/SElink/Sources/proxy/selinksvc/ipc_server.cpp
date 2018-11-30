/**
 *  \file ipc_server.cpp
 *  \author Nicola Ferri
 *  \brief IPC command handlers
 */

#include "selinksvc/ipc_server.h"

#include <cstdint>
#include <vector>
#include <string>
#include <algorithm>
#include <unordered_map>

#include "selinksvc/selinksvc.h"
#include "util/filesystem.h"
#include "proxy/provider/provider_soft.h"
#include "proxy/provider/provider_secube.h"

using selink::service::wfp::ipc;

/*
IPC protocol

- Request packet

    fixed-size packet of 72 B
    header:
        command:ui32
        data-len:ui32 (max value: 64)
    data:
        data[data-len]
        pad[64 - data-len]
        
- Response packet

    variable-size packet
    header:
        status:ui32
        data-len:ui32 (max value: 16*1024)
    data:
        data[data-len]
        
    status values:
        error 0xFFFFFFFF
        ok 0
        function not available 1

- Commands
    
    - reload
        reload rules from file
        
        command 100
        
        request-data: (empty)
        response-data: (empty)
        
    - status
        get provider type and status
        
        command 200
        
        request-data: (empty)
        
        response-data:
            ep_type:ui32
            status:ui32
        
        response-data.status values:
            logged_in 0
            wait_config 1
            error_notfound 100
            error_userpin 200
            error_device 201
            error_unknown 302
            
            (see provider_status.h)
        
    - discover
        get list of connected devices
        
        command 500
        
        request-data: (empty)
        
        response-data:
            device[0]
            device[1]
            ... (a variable number of devices)
        
        response-data.device[n]:
            serial-number[32]
            path-len:ui8 (max value: 31)
            path[path-len]
            
        
    - setdevice
        select and configure one of the connected devices, and attempt login
        
        command 501
        
        request-data:
            serial-number[32]
            pin[pin-size] (max pin-size: 32)
        
        response-data: (empty)
        
        note: query the status using the status command to check whether the login succeeded
        
    - reset
        log out from any connected device and forget configuration
        
        command 502
        
        request-data: (empty)
        
        response-data: (empty)

*/


namespace selink { namespace service { namespace wfp {

    // ipc server command handlers
    static void handle_reload(
        selink::service::wfp::acceptor& proxy_server,
        ipc::request& req,
        ipc::response& resp);
    static void handle_status(
        selink::service::wfp::acceptor& proxy_server,
        ipc::request& req,
        ipc::response& resp);
    static void handle_discover(
        selink::service::wfp::acceptor& proxy_server,
        ipc::request& req,
        ipc::response& resp);
    static void handle_setdevice(
        selink::service::wfp::acceptor& proxy_server,
        ipc::request& req,
        ipc::response& resp);
    static void handle_reset(
        selink::service::wfp::acceptor& proxy_server,
        ipc::request& req,
        ipc::response& resp);

    // ipc server main thread
    void ipc_main(ipc& ipc_listener, wfp::acceptor& proxy_server)
    {
        // register command handler functions
        enum : uint32_t {
            cmd_reload = 100,
            cmd_status = 200,
            cmd_discover = 500,
            cmd_setdevice = 501,
            cmd_reset = 502
        };
        typedef void(*pipe_command_handler)(
            selink::service::wfp::acceptor&,
            ipc::request&,
            ipc::response&);
        std::unordered_map<uint32_t, pipe_command_handler> handlers = {
            { cmd_reload, handle_reload },
            { cmd_status, handle_status },
            { cmd_discover, handle_discover },
            { cmd_setdevice, handle_setdevice },
            { cmd_reset, handle_reset }
        };

        /*    
            Keep processing commands until the service is stopped.

            If the command value is associated with a handler, execute the handler,
            otherwise reply with error status.
        */
        bool stop_received = false;
        while (!stop_received) {
            ipc::response resp;
            /* 
                Wait for a single request from the IPC listener.
                The function is blocking, but will return immediatly (with a command value
                of ipc::request_cmd_stop) when the service is stopped.
                An external application cannot send a stop command.
            */

            ipc::request req = ipc_listener.run_once();
            if (req.command == ipc::request_cmd_stop) {
                stop_received = true;
            }
            else {
                auto it = handlers.find(req.command);
                // check if handler is registered
                if (it == handlers.end()) {
                    resp.status = ipc::response_status_error;
                }
                else {
                    // call handler
                    it->second(proxy_server, req, resp);
                }
            }
            ipc_listener.reply(resp);
        }
    }

    static void handle_reload(
        selink::service::wfp::acceptor& proxy_server,
        ipc::request& req,
        ipc::response& resp)
    {
        rules_update(proxy_server);
    }

    static void handle_status(
        selink::service::wfp::acceptor& proxy_server,
        ipc::request& req,
        ipc::response& resp)
    {
        uint32_t ep_type, status;
        resp.data.resize(8);
        switch (globals.encryption_provider_type) {
        case selink::provider::provider_type::soft:
            ep_type = 0;
            break;
        case selink::provider::provider_type::secube:
            ep_type = 1;
            break;
        default:
            ep_type = 0;
        }
        status = (uint32_t)globals.encryption_provider->status();
        memcpy(&resp.data[0], &ep_type, 4);
        memcpy(&resp.data[4], &status, 4);
    }

    static void handle_discover(
        selink::service::wfp::acceptor& proxy_server,
        ipc::request& req,
        ipc::response& resp)
    {
        for (auto dev : globals.encryption_provider->discover()) {
            resp.data.insert(resp.data.end(), dev.sn.begin(), dev.sn.end());
            std::string path = selink::util::filesystem::path_to_utf8(dev.root);
            if (path.length() > 31)path = path.substr(0, 31);
            resp.data.push_back((uint8_t)path.length());
            resp.data.insert(resp.data.end(), path.begin(), path.end());
        }
    }

    static void handle_setdevice(
        selink::service::wfp::acceptor& proxy_server,
        ipc::request& req,
        ipc::response& resp)
    {
        if (req.size > selink::provider::provider::sn_size) {
            size_t pin_len = req.size - selink::provider::provider::sn_size;
            if (pin_len <= selink::provider::provider::pin_max) {
                std::array<uint8_t, selink::provider::provider::sn_size> dsn;
                std::vector<uint8_t> pin(pin_len);

                std::copy(req.data.begin(),
                    req.data.begin() + selink::provider::provider::sn_size,
                    dsn.begin());
                std::copy(req.data.begin() + selink::provider::provider::sn_size,
                    req.data.begin() + selink::provider::provider::sn_size + pin_len,
                    pin.begin());
                globals.encryption_provider->config(dsn, pin);
                globals.io.post([&](){
                    globals.encryption_provider->login();
                });
            }
        }
    }

    static void handle_reset(
        selink::service::wfp::acceptor& proxy_server,
        ipc::request& req,
        ipc::response& resp)
    {
        globals.io.post([&]() {
            globals.encryption_provider->reset();
        });
    }

} } }  // namespace selink::service::wfp



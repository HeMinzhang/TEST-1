/**
 *  \file ipc_server.h
 *  \author Nicola Ferri
 *  \brief IPC command handlers
 */

#pragma once
#include "util/os.h"

#include "selinksvc/ipc.h"
#include "selinksvc/wfp_acceptor.h"

namespace selink { namespace service { namespace wfp {

    /** \brief Main IPC server thread entry point
     *  \param pipe_listener ipc server object
     *  \param proxy_server proxy server acceptor object
     *  
     *  Handles requests from other processes through the named pipe
     */
    void ipc_main(wfp::ipc& pipe_listener, wfp::acceptor& proxy_server);

} } }  // namespace selink::service::wfp


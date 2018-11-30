/**
 *  \file wfp_acceptor.h
 *  \author Nicola Ferri
 *  \brief Acceptor for SElink service
 *  
 *  Accept connections redirected by the driver and set up the filter component with the
 *    necessary information for bridging.
 */

#pragma once

#include "util/os.h"
#include "util/os_tcpip.h"

#include <cstdint>

#include <boost/asio.hpp>

#include "proxy/filter.h"

#include "selinksvc/wfp_helper.h"
#include "selinksvc/filter_rules.h"

namespace selink { namespace service { namespace wfp {

    using boost::asio::ip::tcp;

    /** This class accepts connections from the local host and starts redirection. 
     *  Source and destination information is retrieved from the driver, so that
     *  filtering and redirection can be applied accordingly.
     */
    class acceptor {
    public:
        /** \brief Initialize a new acceptor
         *  \param io a boost.asio io_service to post asynchronous operations
         *  \param port the port on which the acceptor will listen
         *  \param ep the encryption provider which will encrypt the data
         */
        acceptor(boost::asio::io_service& io, uint16_t port, provider::provider& ep);
        
        /** \brief Begin accepting connections
         *  \return true on success. The function may fail if the port is not available
         *  
         *  The first asychronouse accept operation is posted to the io_service.
         */
        bool start_accept();

        /** A list of rules to decide which action will be taken for each of the
         *  accepted connection. See class filter_rule_table
         */
        filter_rule_table rules;

        void stop();

    private:
        void handle_accept(proxy::filter::pointer flt, const boost::system::error_code& error);
        tcp::acceptor acceptor_;
        provider::provider& ep_;
        boost::asio::strand strand_encrypt;
    };

} } }  // namespace selink::service::wfp






/**
 *  \file wfp_acceptor.cpp
 *  \author Nicola Ferri
 *  \brief Acceptor for SElink service
 */

#include "selinksvc/wfp_acceptor.h"

#include <stdexcept>

#include <boost/thread.hpp>
#include <boost/array.hpp>
#include <boost/bind.hpp>
#include <boost/log/trivial.hpp>

#include "util/filesystem.h"

namespace selink { namespace service { namespace wfp {

    using boost::asio::ip::tcp;

    acceptor::acceptor(boost::asio::io_service& io, uint16_t port, provider::provider& ep)
        : acceptor_(io, tcp::endpoint(boost::asio::ip::address_v4::loopback(), port))
        , ep_(ep), strand_encrypt(io)
    { }

    bool acceptor::start_accept()
    {
        try {
            proxy::filter::pointer flt = proxy::filter::create(acceptor_.get_io_service(), strand_encrypt, ep_);

            acceptor_.async_accept(
                flt->get_fa(),
                boost::bind(
                    &acceptor::handle_accept,
                    this,
                    flt,
                    boost::asio::placeholders::error));
        }
        catch (std::exception&) {
            return false;
        }
        return true;
    }

    void acceptor::handle_accept(proxy::filter::pointer flt, const boost::system::error_code& error)
    {
        int key_id = 1;
        if (!error) {
            wfp_socket_info wfp_info;
            if (!wfp::helper::redirect_setup(flt->get_fa(), flt->get_fb(), &wfp_info)) {
                return;
            }
            filter_entity m;
            m.exe_path = selink::util::filesystem::path_to_utf8(
                wfp::helper::pid_to_path(wfp_info.local_pid));
            m.address = wfp_info.remote_host;
            m.port = wfp_info.remote_port;
            filter_action act = rules.match(m);

            if (act.action == filter_action::allow || act.action == filter_action::encrypt) {
                flt->set_redirect_info(
                    wfp_info.remote_host,
                    wfp_info.remote_port,
                    proxy::filter::side_connect,
                    (act.action == act.encrypt),
                    act.key_id);
                flt->start();
                start_accept();
            }
        }
        else {
            BOOST_LOG_TRIVIAL(trace) << "acceptor::handle_accept error";
        }
    }

    void acceptor::stop()
    {
        acceptor_.close();
    }

} } }  // namespace selink::service::wfp




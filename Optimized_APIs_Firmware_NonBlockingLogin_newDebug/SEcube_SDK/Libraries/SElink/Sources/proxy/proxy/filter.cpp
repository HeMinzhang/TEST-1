/**
 *  \file filter.c
 *  \author Nicola Ferri
 *  \brief Network filter component
 */

#include "proxy/filter.h"

#include <cmath>
#include <array>
#include <algorithm>
#include <stdexcept>

#include <boost/log/trivial.hpp>
#include <boost/bind.hpp>

#include "util/rng.h"

namespace selink { namespace proxy {

        void filter::start()
        {
            /* Choose which socket we need to connect, depending on the side.
               A is always the local endpoint, and B is always the remote endpoint,
               therefore fb is encrypted (possibly, in encrypt mode), fb is not.
               However, if we are bridging for the server, we must connect to the local
               endpoint, which is accepting connections; if we are bridging for the client,
               we must connect to the remote endpoint, the one for which the
               redirected connection was intended.
            */
            boost::asio::ip::tcp::socket& conn_socket =
                (redirect_info.side == side_connect) ?
                (fb) : (fa);
            if (redirect_info.encrypt && (ep_.status() == selink::provider::wait_config)) {
                // encrypt mode but provider not ready
                BOOST_LOG_TRIVIAL(error) << "Encryption provider not ready (wait_config)";
                boost::mutex::scoped_lock lock(mut);   
                try {
                    if (conn_socket.is_open()) {
                        conn_socket.shutdown(tcp::socket::shutdown_both);
                        conn_socket.close();
                    }
                } catch(std::exception&) {}
                return;
            }

            ep_ref = ep_.ref;

            // create endpoint from redirect info
            boost::asio::ip::address addr;
            if (redirect_info.host_ip.empty()) {
                addr = boost::asio::ip::address::from_string(redirect_info.host);
            }
            else if (redirect_info.host_ip.size() == 4) {
                std::array<uint8_t, 4> tmp;
                std::copy(redirect_info.host_ip.begin(), redirect_info.host_ip.end(), tmp.begin());
                addr = boost::asio::ip::address_v4(tmp);
            }
            else if (redirect_info.host_ip.size() == 16) {
                std::array<uint8_t, 16> tmp;
                std::copy(redirect_info.host_ip.begin(), redirect_info.host_ip.end(), tmp.begin());
                addr = boost::asio::ip::address_v6(tmp);
            }
            else {
                BOOST_LOG_TRIVIAL(error) << "Redirect information not valid";
                stop();
                return;
            }

            if (redirect_info.side != side_connect && redirect_info.side != side_accept) {
                // this should not happen
                stop();
                return;
            }


            if (redirect_info.encrypt) {
                // allocate buffers needed for encryption
                eb = std::unique_ptr<encryption_buffers>(new encryption_buffers());
                
                // follow encrypt mode path
                conn_socket.async_connect(
                    tcp::endpoint(addr, redirect_info.port),
                    strand_encrypt_.wrap(
                        boost::bind(
                            &filter::handle_connect_encrypt,
                            shared_from_this(),
                            boost::asio::placeholders::error)));
            }
            else {
                // follow bypass mode path
                conn_socket.async_connect(
                    tcp::endpoint(addr, redirect_info.port),
                    boost::bind(
                        &filter::handle_connect_bypass,
                        shared_from_this(),
                        boost::asio::placeholders::error));
            }
        }

        tcp::socket& filter::get_fa()
        {
            return fa;
        }
        tcp::socket& filter::get_fb()
        {
            return fb;
        }

        filter::pointer filter::create(boost::asio::io_service& io, boost::asio::strand& strand_encrypt, provider::provider& ep)
        {
            return filter::pointer(new filter(io, strand_encrypt, ep));
        }

        filter::filter(boost::asio::io_service& io, boost::asio::strand& strand_encrypt, provider::provider& ep)
            : fa(io), fb(io), ep_(ep), eb(nullptr), strand_encrypt_(strand_encrypt),
            buf_ab_raw(1024), buf_ba_raw(1024)
        { }

        void filter::set_redirect_info(std::string host,
            uint16_t port, int side, bool encrypt, int key_id)
        {
            redirect_info.host_ip.clear();
            redirect_info.host = host;
            redirect_info.port = port;
            redirect_info.side = side;
            redirect_info.encrypt = encrypt;
            redirect_info.key_id = key_id;
        }

        void filter::set_redirect_info(std::vector<uint8_t> host,
            uint16_t port, int side, bool encrypt, int key_id)
        {
            redirect_info.host.clear();
            redirect_info.host_ip = host;
            redirect_info.port = port;
            redirect_info.side = side;
            redirect_info.encrypt = encrypt;
            redirect_info.key_id = key_id;
        }


        void filter::handle_connect_encrypt(const boost::system::error_code& error) {
            if (error || ep_.ref != ep_ref) {
                stop();
            }
            else {
                // create encrypting session A->B
                selink::util::rng::rand_bytes(eb->nonce_ab, provider::provider::nonce_size);
                session_ab = ep_.create_session(
                    redirect_info.key_id, eb->nonce_ab, provider::session::direction::encrypt);

                // send nonce to B
                boost::asio::async_write(fb,
                    boost::asio::buffer(eb->nonce_ab, provider::provider::nonce_size),
                    strand_encrypt_.wrap(
                        boost::bind(&filter::handle_fb_sent_nonce,
                        shared_from_this(),
                        boost::asio::placeholders::error)));


                // receive nonce from B
                boost::asio::async_read(fb,
                    boost::asio::buffer(eb->nonce_ba, provider::provider::nonce_size),
                    strand_encrypt_.wrap(
                        boost::bind(
                            &filter::handle_fb_read_nonce,
                            shared_from_this(),
                            boost::asio::placeholders::error,
                            boost::asio::placeholders::bytes_transferred)));
            }
        }


        // A <-> F
        void filter::handle_fa_sent_data(
            const boost::system::error_code& error)
        {
            if (error || ep_.ref != ep_ref) {
                stop();
                return;
            }

            // read iv + header from B
            boost::asio::async_read(fb,
                boost::asio::buffer(buf_ba_raw, eb->bytes_needed_for_header),
                strand_encrypt_.wrap(
                    boost::bind(
                        &filter::handle_fb_read_hdr,
                        shared_from_this(),
                        boost::asio::placeholders::error,
                        boost::asio::placeholders::bytes_transferred)));
        }

        void filter::handle_fa_read_data(
            const boost::system::error_code& error,
            std::size_t bytes_transferred)
        {
            if (error || ep_.ref != ep_ref) {
                stop();
                return;
            }
            // if the buffer was filled entirely, increase its size
            if ((bytes_transferred == buf_ab_raw.size() - encryption_overhead) &&
                (buf_ab_raw.size() < buffer_max))
            {
                buf_ensure(buf_ab_raw, buf_ab_raw.size() + 1);
            }
            buf_ensure(eb->buf_ab_enc, buf_ab_raw.size());

            // encrypt
            int enc_size = packet_encrypt(bytes_transferred, &buf_ab_raw[0], &(eb->buf_ab_enc[0]));
            if (enc_size < 1) {
                stop();
                return;
            }
            // send data to B
            boost::asio::async_write(fb,
                boost::asio::buffer(eb->buf_ab_enc, enc_size),
                strand_encrypt_.wrap(
                    boost::bind(&filter::handle_fb_sent_data,
                        shared_from_this(),
                        boost::asio::placeholders::error)));
        }

        // F <-> B
        void filter::handle_fb_sent_nonce(
            const boost::system::error_code& error)
        {
            if (error || ep_.ref != ep_ref) {
                stop();
                return;
            }
            // read some data from A
            fa.async_read_some(
                boost::asio::buffer(
                    &buf_ab_raw[header_size], buf_ab_raw.size() - encryption_overhead),
                strand_encrypt_.wrap(
                    boost::bind(
                        &filter::handle_fa_read_data,
                        shared_from_this(),
                        boost::asio::placeholders::error,
                        boost::asio::placeholders::bytes_transferred)));
        }
        void filter::handle_fb_sent_data(
            const boost::system::error_code& error)
        {
            if (error || ep_.ref != ep_ref) {
                stop();
                return;
            }
            // read some data from A
            fa.async_read_some(
                boost::asio::buffer(
                    &buf_ab_raw[header_size], buf_ab_raw.size() - encryption_overhead),
                strand_encrypt_.wrap(
                    boost::bind(
                        &filter::handle_fa_read_data,
                        shared_from_this(),
                        boost::asio::placeholders::error,
                        boost::asio::placeholders::bytes_transferred)));
        }

        void filter::handle_fb_read_nonce(
            const boost::system::error_code& error,
            std::size_t bytes_transferred)
        {
            if (error || ep_.ref != ep_ref) {
                stop();
                return;
            }
            if (bytes_transferred != provider::provider::nonce_size) {
                stop();
                return;
            }
            // create decrypting session B->A
            session_ba = ep_.create_session(
                redirect_info.key_id, eb->nonce_ba, provider::session::direction::decrypt);

            // read iv + header from B
            boost::asio::async_read(fb,
                boost::asio::buffer(buf_ba_raw, eb->bytes_needed_for_header),
                strand_encrypt_.wrap(
                    boost::bind(
                        &filter::handle_fb_read_hdr,
                        shared_from_this(),
                        boost::asio::placeholders::error,
                        boost::asio::placeholders::bytes_transferred)));
        }

        void filter::handle_fb_read_hdr(
            const boost::system::error_code& error,
            std::size_t bytes_transferred)
        {
            if (error || ep_.ref != ep_ref) {
                stop();
                return;
            }
            if (bytes_transferred != eb->bytes_needed_for_header) {
                stop();
                return;
            }

            // decrypt header
            buf_ensure(eb->buf_ba_dec, buf_ba_raw.size());
            packet_decrypt_hdr(&buf_ba_raw[0], &(eb->buf_ba_dec[0]));
            if (eb->packet_decrypt_state.error != eb->packet_decrypt_state.error_none) {
                stop();
                return;
            }

            // compute bytes needed to complete the packet
            eb->buf_ba_needed = eb->packet_decrypt_state.packet_size - eb->bytes_needed_for_header;
            // encryption::session::padded_size(message_size) + encryption::session::auth_size;

            // receive packet message+auth
            buf_ensure(buf_ba_raw, eb->packet_decrypt_state.packet_size);
            boost::asio::async_read(fb,
                boost::asio::buffer(&buf_ba_raw[eb->bytes_needed_for_header], eb->buf_ba_needed),
                strand_encrypt_.wrap(
                    boost::bind(
                        &filter::handle_fb_read_data,
                        shared_from_this(),
                        boost::asio::placeholders::error,
                        boost::asio::placeholders::bytes_transferred)));
        }

        void filter::handle_fb_read_data(
            const boost::system::error_code& error,
            std::size_t bytes_transferred)
        {
            if (error || ep_.ref != ep_ref) {
                stop();
                return;
            }
            if (bytes_transferred != eb->buf_ba_needed) {
                stop();
                return;
            }

            // decrypt
            buf_ensure(eb->buf_ba_dec, buf_ba_raw.size());
            packet_decrypt_msg(&buf_ba_raw[0], &(eb->buf_ba_dec[0]));
            if (eb->packet_decrypt_state.error != eb->packet_decrypt_state.error_none) {
                stop();
                return;
            }

            // send data to A
            boost::asio::async_write(fa,
                boost::asio::buffer(
                    &(eb->buf_ba_dec[header_size]), eb->packet_decrypt_state.message_size),
                strand_encrypt_.wrap(
                    boost::bind(&filter::handle_fa_sent_data,
                        shared_from_this(),
                        boost::asio::placeholders::error)));
        }


        // --- Bypass ---

        void filter::handle_connect_bypass(const boost::system::error_code& error)
        {
            if (error) {
                BOOST_LOG_TRIVIAL(trace) << "handle_connect_bypass error " << error.value();
                stop();
                return;
            }

            // start reading from A and B
            fa.async_read_some(
                boost::asio::buffer(buf_ab_raw),
                boost::bind(
                    &filter::handle_fa_read_bypass,
                    shared_from_this(),
                    boost::asio::placeholders::error,
                    boost::asio::placeholders::bytes_transferred));
            fb.async_read_some(
                boost::asio::buffer(buf_ba_raw),
                boost::bind(
                    &filter::handle_fb_read_bypass,
                    shared_from_this(),
                    boost::asio::placeholders::error,
                    boost::asio::placeholders::bytes_transferred));
        }

        void filter::handle_fa_sent_bypass(
            const boost::system::error_code& error)
        {
            if (error) {
                BOOST_LOG_TRIVIAL(trace) << "handle_fa_sent_bypass error " << error.value();
                stop();
                return;
            }

            fb.async_read_some(
                boost::asio::buffer(buf_ba_raw),
                boost::bind(
                    &filter::handle_fb_read_bypass,
                    shared_from_this(),
                    boost::asio::placeholders::error,
                    boost::asio::placeholders::bytes_transferred));
        }
        void filter::handle_fb_sent_bypass(
            const boost::system::error_code& error)
        {
            if (error) {
                BOOST_LOG_TRIVIAL(trace) << "handle_fb_sent_bypass error " << error.value();
                stop();
                return;
            }

            fa.async_read_some(
                boost::asio::buffer(buf_ab_raw),
                boost::bind(
                    &filter::handle_fa_read_bypass,
                    shared_from_this(),
                    boost::asio::placeholders::error,
                    boost::asio::placeholders::bytes_transferred));
        }
        void filter::handle_fa_read_bypass(
            const boost::system::error_code& error,
            std::size_t bytes_transferred)
        {
            if (error) {
                BOOST_LOG_TRIVIAL(trace) << "handle_fa_read_bypass error " << error.value();
                stop();
                return;
            }
            // if the buffer was filled entirely, increase its size
            if (bytes_transferred == buf_ab_raw.size() &&
                (buf_ab_raw.size() < buffer_max))
            {
                buf_ensure(buf_ab_raw, buf_ab_raw.size() + 1);
            }
            // send data to B
            boost::asio::async_write(fb,
                boost::asio::buffer(buf_ab_raw, bytes_transferred),
                boost::bind(&filter::handle_fb_sent_bypass,
                    shared_from_this(),
                    boost::asio::placeholders::error));
        }
        void filter::handle_fb_read_bypass(
            const boost::system::error_code& error,
            std::size_t bytes_transferred)
        {
            if (error) {
                BOOST_LOG_TRIVIAL(trace) << "handle_fb_read_bypass error " << error.value();
                stop();
                return;
            }
            // if the buffer was filled entirely, increase its size
            if (bytes_transferred == buf_ba_raw.size() &&
                (buf_ba_raw.size() < buffer_max))
            {
                buf_ensure(buf_ba_raw, buf_ba_raw.size() + 1);
            }
            // send data to A
            boost::asio::async_write(fa,
                boost::asio::buffer(buf_ba_raw, bytes_transferred),
                boost::bind(&filter::handle_fa_sent_bypass,
                    shared_from_this(),
                    boost::asio::placeholders::error));
        }


        void filter::stop()
        {
            boost::mutex::scoped_lock lock(mut);
            try {
                if (fa.is_open()) {
                    fa.shutdown(tcp::socket::shutdown_both);
                    fa.close();
                }
            }
            catch (std::exception&) {}
            try {
                if (fb.is_open()) {
                    fb.shutdown(tcp::socket::shutdown_both);
                    fb.close();
                }
            }
            catch (std::exception&) {}
        }

        void filter::packet_decrypt_hdr(const uint8_t* data_in, uint8_t* data_out) {
            if (!session_ba) {
                eb->packet_decrypt_state.error = eb->packet_decrypt_state.error_provider;
                return;
            }
            size_t nblocks = provider::session::padded_size(header_size) /
                provider::session::block_size;
            provider::session::error status;

            status = session_ba->reset(data_in);
            if (status != provider::session::error::ok) {
                BOOST_LOG_TRIVIAL(error) << "filter/decrypt reset error";
                eb->packet_decrypt_state.error = eb->packet_decrypt_state.error_provider;
            }
            status = session_ba->update(nblocks, data_in + provider::session::iv_size, data_out);
            if (status != provider::session::error::ok) {
                BOOST_LOG_TRIVIAL(error) << "filter/decrypt update error";
                eb->packet_decrypt_state.error = eb->packet_decrypt_state.error_provider;
            }
            eb->packet_decrypt_state.bytes_decrypted = nblocks*provider::session::block_size;
            eb->packet_decrypt_state.message_size =
                (((size_t)data_out[0]) << 8) | ((size_t)data_out[1]);
            eb->packet_decrypt_state.packet_size =
                provider::session::iv_size +
                provider::session::padded_size(
                    eb->packet_decrypt_state.message_size +
                    header_size) +
                provider::session::auth_size;
            eb->packet_decrypt_state.error = eb->packet_decrypt_state.error_none;
        }

        void filter::packet_decrypt_msg(const uint8_t* data_in, uint8_t* data_out) {
            if (!session_ba) {
                eb->packet_decrypt_state.error = eb->packet_decrypt_state.error_provider;
                return;
            }
            provider::session::error status;

            size_t nbytes =
                provider::session::padded_size(
                    eb->packet_decrypt_state.message_size + header_size) -
                eb->packet_decrypt_state.bytes_decrypted;
            size_t nblocks = nbytes / 16;
            status = session_ba->update(nblocks,
                data_in + provider::session::iv_size + eb->packet_decrypt_state.bytes_decrypted,
                data_out + eb->packet_decrypt_state.bytes_decrypted);
            if (status != provider::session::error::ok) {
                BOOST_LOG_TRIVIAL(error) << "filter/decrypt update error";
                eb->packet_decrypt_state.error = eb->packet_decrypt_state.error_provider;
            }
            status = session_ba->finalize(eb->auth_ba);
            if (status != provider::session::error::ok) {
                if (status == provider::session::error::ok) {
                    eb->packet_decrypt_state.error = eb->packet_decrypt_state.error_auth;
                    BOOST_LOG_TRIVIAL(trace) << "filter/decrypt auth mismatch";
                }
                else {
                    eb->packet_decrypt_state.error = eb->packet_decrypt_state.error_provider;
                    BOOST_LOG_TRIVIAL(error) << "filter/decrypt finalize error";
                }
                return;
            }
        }

        int filter::packet_encrypt(size_t len, uint8_t* data_in, uint8_t* data_out) {
            if (!session_ab) {
                return -1;
            }
            if (len > message_max) {
                return -1;
            }
            provider::session::error status;

            util::rng::rand_bytes(data_out, provider::session::iv_size);
            status = session_ab->reset(data_out);
            if (status != provider::session::error::ok) {
                return -1;
            }

            // write header
            data_in[0] = (uint8_t)((len >> 8) & 0xFF);
            data_in[1] = (uint8_t)(len & 0xFF);
            data_in[2] = data_in[0];
            data_in[3] = data_in[1];

            // write padding
            size_t nbytes = provider::session::padded_size(header_size + len);
            size_t npadding = nbytes - (header_size + len);
            util::rng::rand_bytes(data_in + header_size + len, npadding);

            // encrypt
            size_t nblocks = nbytes / 16;
            status = session_ab->update(nblocks, data_in, data_out + provider::session::iv_size);
            if (status != provider::session::error::ok) {
                BOOST_LOG_TRIVIAL(error) << "filter/encrypt update error";
                return -1;
            }

            // authenticate
            status = session_ab->finalize(data_out + provider::session::iv_size + nbytes);
            if (status != provider::session::error::ok) {
                BOOST_LOG_TRIVIAL(error) << "filter/encrypt finalize error";
                return -1;
            }

            // return final size (iv+ciphertext+auth)
            size_t final_size =
                provider::session::iv_size +
                nbytes +
                provider::session::auth_size;
            
            return (int)final_size;
        }

        void filter::buf_ensure(std::vector<uint8_t>& vec, size_t needed) {
            size_t next = (size_t)pow(2, ceil(log(needed) / log(2)));
            vec.resize(next);
        }

        void filter::dbg_test(provider::provider& ep, int ep_id) {
            uint8_t buf[1024], bufenc[1024], bufdec[1024];
            boost::asio::io_service io;
            boost::asio::strand strand_encrypt(io);
            auto flt = filter::create(io, strand_encrypt, ep);
            flt->eb = std::unique_ptr<encryption_buffers>(new encryption_buffers());
            flt->session_ab = ep.create_session(
                ep_id, provider::provider::test_nonce, provider::session::direction::encrypt);
            assert(flt->session_ab != nullptr);
            flt->session_ba = ep.create_session(
                ep_id, provider::provider::test_nonce, provider::session::direction::decrypt);
            assert(flt->session_ba != nullptr);

            std::string message = "zergrush";

            memcpy(buf + header_size, message.c_str(), message.length());
            flt->packet_encrypt(message.length(), buf, bufenc);
            flt->packet_decrypt_hdr(bufenc, bufdec);
            flt->packet_decrypt_msg(bufenc, bufdec);

            assert(buf[provider::session::iv_size + header_size] ==
                bufdec[provider::session::iv_size + header_size]);
        }

} }  // namespace selink::proxy

/**
 *  \file filter.h
 *  \author Nicola Ferri
 *  \brief Network filter component
 *
 *  This component bridges two sockets, encrypting or decrypting traffic as needed.
 */

#pragma once
#include "util/os.h"

#include <cstdint>
#include <memory>
#include <string>
#include <vector>

#include <boost/thread.hpp>
#include <boost/asio.hpp>
#include <boost/enable_shared_from_this.hpp>

#include "provider/provider.h"

namespace selink { namespace proxy {

    using boost::asio::ip::tcp;

    /** This class implements the logic for communication filtering.
     *  It takes two sockets, FA (filter <-> A) and FB (filter <-> B)
     *  and bridges the unencrypted connection FA to the encrypted connection FB.
     *  Depending on the configuration, it may also proxy the connection
     *  as is (bypass mode)
     */
    class filter : public boost::enable_shared_from_this<filter> {
    public:
        /** Protocol constants */
        enum {
            buffer_max = (64 * 1024),
            header_size = 4,
            encryption_overhead = (
                header_size + provider::session::iv_size +
                provider::session::iv_size),
            message_max = (buffer_max - encryption_overhead)
        };
        
        enum {
            /** server side proxy */
            side_accept,
            /** client side proxy */
            side_connect
        };
        typedef boost::shared_ptr<filter> pointer;

        /** \brief start the bridging.
         *
         *  The filter will connect to the other endpoint and filter
         *  any communication
         */
        void start();

        /** \brief Get socket FA */
        tcp::socket& get_fa();

        /** \brief Get socket FB */
        tcp::socket& get_fb();

        /** \brief Create a new filter
         *  \param io boost.asio io service
         *  \param strand_encrypt common strand for all encrypted connections
         *  \param ep encryption provider
         *  
         *  If the encryption provider does not support concurrent operations,
         *  it is important to always specify the same strand, to ensure mutual exclusion
         *  of the operations using the provider.
         */
        static pointer create(
            boost::asio::io_service& io, boost::asio::strand& strand_encrypt, provider::provider& ep);

        /** \brief Set parameters for redirection
         *  \param host the host to which the connection will be redirected
         *  \param port the port to which the connection will be redirected
         *  \param encrypt if true, the connection will be encrypted
         *  \param key_id key to be used to encrypt the connection
         */
        void set_redirect_info(std::string host,
            uint16_t port, int side, bool encrypt, int key_id);

        /** \brief Set parameters for redirection
        *  \param host ip address of the host to which the connection will be redirected
        *  \param port the port to which the connection will be redirected
        *  \param encrypt if true, the connection will be encrypted
        *  \param key_id key to be used to encrypt the connection
        */
        void set_redirect_info(std::vector<uint8_t> host,
            uint16_t port, int side, bool encrypt, int key_id);

        // TODO remove
        static void dbg_test(provider::provider& ep, int ep_id);

    private:

        /** Redirection parameters needed to setup the bridge */  
        struct redirect_info_t {
            int side;
            std::string host;
            std::vector<uint8_t> host_ip;
            uint16_t port{ 0 };
            bool encrypt{ false };
            int key_id{ 0 };
        } redirect_info;

        tcp::socket fa;
        tcp::socket fb;
        provider::provider& ep_;
        unsigned int ep_ref{ 0 };

        boost::mutex mut;

        /** See create(). */
        filter(boost::asio::io_service& io, boost::asio::strand& strand_encrypt, provider::provider& ep);


        /** @{
         *  Handlers for encryption mode path
         */
        void handle_connect_encrypt(const boost::system::error_code& error);

        // A <-> F
        void handle_fa_sent_data(
            const boost::system::error_code& error);
        void handle_fa_read_data(
            const boost::system::error_code& error,
            std::size_t bytes_transferred);

        // F <-> B
        void handle_fb_sent_nonce(
            const boost::system::error_code& error);
        void handle_fb_sent_data(
            const boost::system::error_code& error);
        void handle_fb_read_nonce(
            const boost::system::error_code& error,
            std::size_t bytes_transferred);
        void handle_fb_read_hdr(
            const boost::system::error_code& error,
            std::size_t bytes_transferred);
        void handle_fb_read_data(
            const boost::system::error_code& error,
            std::size_t bytes_transferred);
        /** @} */


        /** @{
        *  Handlers for bypass mode path
        */
        void handle_connect_bypass(const boost::system::error_code& error);
        void handle_fa_sent_bypass(
            const boost::system::error_code& error);
        void handle_fa_read_bypass(
            const boost::system::error_code& error,
            std::size_t bytes_transferred);
        void handle_fb_sent_bypass(
            const boost::system::error_code& error);
        void handle_fb_read_bypass(
            const boost::system::error_code& error,
            std::size_t bytes_transferred);
        /** @} */

        /** Structures for encryption mode */
        struct encryption_buffers {
            struct {
                enum { error_none = 0, error_auth, error_provider, error_header, error_init };
                int error{ error_init };
                size_t bytes_decrypted{ 0 };
                size_t message_size{ 0 };
                size_t packet_size{ 0 };
            } packet_decrypt_state;

            size_t bytes_needed_for_header;

            uint8_t nonce_ab[provider::provider::nonce_size];
            uint8_t nonce_ba[provider::provider::nonce_size];
            uint8_t auth_ba[provider::session::auth_size];

            std::vector<uint8_t> buf_ab_enc;
            std::vector<uint8_t> buf_ba_dec;
            size_t buf_ba_needed;
            size_t buf_ba_message_size;

            encryption_buffers() :
                buf_ab_enc(1024), buf_ba_dec(1024),
                buf_ba_needed(0), buf_ba_message_size(0)
            {
                bytes_needed_for_header =
                    provider::session::iv_size +
                    provider::session::padded_size(header_size);
            }
        };

        std::unique_ptr<encryption_buffers> eb{ nullptr };
        boost::asio::strand& strand_encrypt_;
        
        /** Session A-to-B */
        std::shared_ptr<provider::session> session_ab;

        /** Session B-to-A */
        std::shared_ptr<provider::session> session_ba;

        /** Raw data buffers */
        std::vector<uint8_t> buf_ab_raw;
        std::vector<uint8_t> buf_ba_raw;

        /** \brief Decrypt packet header to extract size */
        void packet_decrypt_hdr(const uint8_t* data_in, uint8_t* data_out);
        
        /** \brief Decrypt packet message */
        void packet_decrypt_msg(const uint8_t* data_in, uint8_t* data_out);
        
        /** \brief Encrypt full packet */
        int packet_encrypt(size_t len, uint8_t* data_in, uint8_t* data_out);

        /** \brief Close both sockets and terminate bridging */
        void stop();

        /** \brief Ensure buffer capacity
         *  \param vec vector of uint8_t, its size must be a power of 2
         *  \param needed bytes needed in buffer
         *  
         *  the buffer will be resized to a capacity greater than or equal to 'needed'.
         *  Increments are in powers of 2.
         */
        static void buf_ensure(std::vector<uint8_t>& vec, size_t needed);
    };

} }  // namespace selink::proxy





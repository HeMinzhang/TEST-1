/**
 *  \file provider.h
 *  \author Nicola Ferri
 *  \brief provider and session interfaces
 *
 *  Any implementation to be used as encryption provider must derive and implement
 *    the provider and session interfaces.
 */

#pragma once
#include "util/os.h"

#include <cstdint>
#include <memory>
#include <vector>
#include <array>

#include <boost/filesystem.hpp>

#include "proxy/provider/provider_status.h"

namespace selink { namespace provider {

    /** Session interface */
    class session {
    public:

        enum {
            block_size = 16,
            auth_size = 16,
            iv_size = 16,
            key_size = 32
        };

        /** Session direction */
        enum class direction {
            encrypt,
            decrypt
        };

        /** Session error codes */
        enum class error {
            ok,  ///< No error
            provider,  ///< Device failure
            auth,  ///< The authentication tag does not match
            params  ///< Invalid parameters
        };

        /** \brief Session constructor
         *  \param direction either direction::encrypt or direction::decrypt
         */
        explicit session(direction dir);
        
        virtual ~session();

        /** \brief Encrypt or decrypt a chunk
         *  \param nblocks number of blocks to encrypt
         *  \param data_in input byte array
         *  \param data_out output byte array
         *  \return error code
         */
        virtual session::error update(
            size_t nblocks, const uint8_t* data_in, uint8_t* data_out) = 0;

        /** \brief Produce authentication tag
         *  \param auth output byte array
         *  \return error code
         */
        virtual session::error finalize(uint8_t* auth) = 0;

        /** \brief Set the iv and reset authentication
         *  \param iv initialization vector byte array
         *  \return error code
         */
        virtual session::error reset(const uint8_t* iv) = 0;

        /** \brief Ceil to the nearest multiple of block size
         */
        static size_t padded_size(size_t size);

        static const uint8_t test_key[32];
        static const uint8_t test_iv[16];

    protected:
        direction direction_;
    };


    /** Encryption provider interface */
    class provider {
    public:
        enum {
            nonce_size = session::key_size,
            pin_max = 32,
            sn_size = 32
        };

        /** A generic device, identified by serial number and path */
        struct device {
            std::array<uint8_t, provider::sn_size> sn;
            boost::filesystem::path root;
        };


        provider();
        virtual ~provider();

        /** \brief Provider's current status
            \return One of the values in enum provider_status
        */
        virtual int status() = 0;

        /** \brief Configure the provider
         *  \param sn serial number
         *  \param pin user pin
         *  
         *  The provided parameters are needed to identify a device and log into it.
         *  This function should not necessarily attempt to connect to the login
         *  immediately.
         *  Must guarantee thread-safety.
         */
        virtual void config(
            std::array<uint8_t, sn_size> sn, std::vector<uint8_t> pin) = 0;

        /** \brief Perform login
         *  
         *  Try to log into the device, set the provider status accordingly.
         *  Must NOT guarantee thread-safety.
         */
        virtual void login() = 0;

        /** \brief Create a new session
        *
        *  Must NOT guarantee thread-safety.
        */
        virtual std::shared_ptr<session> create_session(
            int id, const uint8_t* nonce, session::direction dir) = 0;

        /** \brief Discover devices
        *  
        *  Must guarantee thread-safety.
        */
        virtual std::vector<device> discover() = 0;

        /** \brief Clear configuration
        *
        *  Clear configuration parameters, log out, set the provider status accordingly.
        *  Should increase 'ref' to invalidate all previous sessions.
        *  Must NOT guarantee thread-safety.
        */
        virtual void reset() = 0;

        unsigned int ref{ 0 };

        static const uint8_t test_nonce[nonce_size];
    };

    /** All types of supported providers */
    enum struct provider_type{
        soft,
        secube
    };

} }

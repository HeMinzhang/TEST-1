/**
 *  \file provider_soft.h
 *  \author Nicola Ferri
 *  \brief Software provider implementation
 */

#pragma once
#include "util/os.h"

#include <memory>
#include <array>
#include <vector>
#include <unordered_map>

#include <boost/thread.hpp>
#include <boost/filesystem.hpp>

#include "secube/aes256.h"
#include "secube/sha256.h"
#include "secube/pbkdf2.h"
#include "proxy/provider/provider.h"

namespace selink { namespace provider {

    class session_soft : public session
    {
    public:
        session_soft(uint8_t* session_key, session::direction dir);
        ~session_soft() override;
        session::error update(size_t nblocks,
            const uint8_t* data_in, uint8_t* data_out) override;
        session::error finalize(uint8_t* auth) override;
        session::error reset(const uint8_t* iv) override;

    private:
        B5_tAesCtx aes_ctx;
        B5_tHmacSha256Ctx hmac_ctx;
        uint8_t key1[session::key_size];
		uint8_t key2[session::key_size];
        uint8_t auth_[32];
        int direction_;
    };

    class provider_soft : public provider
    {
    public:
        enum {
            key_size = 32
        };
        struct key_entry {
            std::array<uint8_t, key_size> data;
            uint32_t val{ 0 };
        };
        typedef std::unordered_map<int, key_entry> key_collection;

        static key_collection keys_from_file(boost::filesystem::path path);

        provider_soft();
        ~provider_soft() override;
        void set_keys(key_collection newkeys);
        int status() override;
        void config(std::array<uint8_t, provider::sn_size> sn,
            std::vector<uint8_t> pin) override;
        void login() override;

        std::shared_ptr<session> create_session(
            int id, const uint8_t* nonce, session::direction dir) override;
        std::vector<provider::device> discover() override;
        void reset() override;

        void dbg_setup_test();
    private:
        struct key_context {
            int id;
            key_entry keydata;
        };
        typedef std::unordered_map<int, key_context> key_table;
        key_table keys;

        // wrap any call in mutex(mut)
        key_context* get_key(int id);

        boost::mutex mut;
        int status_;
    };

} }  // namespace selink::provider


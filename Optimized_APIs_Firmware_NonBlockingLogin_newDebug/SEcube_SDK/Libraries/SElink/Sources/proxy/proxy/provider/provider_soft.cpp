/**
 *  \file provider_soft.cpp
 *  \author Nicola Ferri
 *  \brief Software provider implementation
 */

#include "proxy/provider/provider_soft.h"

#include <string>
#include <algorithm>
#include <stdexcept>

#include <boost/log/trivial.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/algorithm/hex.hpp>
#include <boost/filesystem/fstream.hpp>

namespace selink { namespace provider {

    session_soft::session_soft(uint8_t* session_key, session::direction dir) :
        session(dir)
    {
		memcpy(key1, session_key + 0 * session::key_size, session::key_size);
		memcpy(key2, session_key + 1 * session::key_size, session::key_size);

        if (session::direction_ == session::direction::encrypt) {
            B5_Aes256_Init(&aes_ctx, key1, session::key_size, B5_AES256_CBC_ENC);
        }
        else {
            B5_Aes256_Init(&aes_ctx, key1, session::key_size, B5_AES256_CBC_DEC);
        }
    }

    session_soft::~session_soft()
    { }

    session::error session_soft::update(
        size_t nblocks, const uint8_t * data_in, uint8_t* data_out)
    {
        if (session::direction_ == session::direction::encrypt) {
            B5_Aes256_Update(&aes_ctx, data_out, (uint8_t*)data_in, (int16_t)nblocks);
            B5_HmacSha256_Update(&hmac_ctx, data_in, (int16_t)(nblocks*session::block_size));
        }
        else {
            B5_Aes256_Update(&aes_ctx, (uint8_t*)data_in, data_out, (int16_t)nblocks);
            B5_HmacSha256_Update(&hmac_ctx, data_out, (int16_t)(nblocks*session::block_size));
        }
        return session::error::ok;
    }

    session::error session_soft::finalize(uint8_t* auth)
    {
        B5_HmacSha256_Finit(&hmac_ctx, auth_);
        memcpy(auth, auth_ + 16, 16);
        return session::error::ok;
    }

    session::error session_soft::reset(const uint8_t* iv)
    {
        B5_HmacSha256_Init(&hmac_ctx, key2, session::key_size);
        B5_Aes256_SetIV(&aes_ctx, iv);
		B5_HmacSha256_Update(&hmac_ctx, iv, session::iv_size);
        return session::error::ok;
    }

    provider_soft::provider_soft()
        : provider(), status_(logged_in)
    { }

    provider_soft::~provider_soft()
    { }

    void provider_soft::set_keys(key_collection newkeys)
    {
        boost::mutex::scoped_lock lock1(mut);
        keys.clear();
        for (auto key : newkeys) {
            key_context second;
            second.id = key.first;
            second.keydata = key.second;
            keys.insert({ key.first, second });
        }
    }

    void provider_soft::dbg_setup_test()
    {
        provider_soft::key_collection keys;
        provider_soft::key_entry key;
        memcpy(&key.data[0], session::test_key, session::key_size);
        key.val = (uint32_t)time(0);
        keys.insert({ 1, key });
        set_keys(keys);
    }

    provider_soft::key_context* provider_soft::get_key(int id)
    {
        auto key = keys.find(id);
        if (key == keys.end()) {
            return nullptr;
        }
        key_context* ctx = &(key->second);
        return ctx;
    }


    int provider_soft::status()
    {
        return status_;
    }
    void provider_soft::config(
        std::array<uint8_t, provider::sn_size> sn, std::vector<uint8_t> pin)
    { }
    void provider_soft::login()
    { }
    std::vector<provider::device> provider_soft::discover()
    {
        return std::vector<provider::device>();
    }
    void provider_soft::reset()
    { }

    std::shared_ptr<session> provider_soft::create_session(
        int id, const uint8_t* nonce, session::direction dir)
    {
        boost::mutex::scoped_lock lock1(mut);

        key_context* ctx = get_key(id);
        if (ctx == nullptr) {
            return std::shared_ptr<session>(nullptr);
        }

        uint8_t session_key[2*session::key_size];
		PBKDF2HmacSha256(&(ctx->keydata.data[0]), session::key_size, nonce, provider::nonce_size, 1, session_key, 2 * session::key_size);

        BOOST_LOG_TRIVIAL(trace) <<
            "nonce " << (unsigned)nonce[0] << "," << (unsigned)nonce[1] <<
            " session " << (unsigned)session_key[0] << "," << (unsigned)session_key[1] <<
            " " << ((dir == session::direction::encrypt) ? "encrypt" : "decrypt");

        return std::shared_ptr<session>(new session_soft(session_key, dir));
    }

    provider_soft::key_collection provider_soft::keys_from_file(boost::filesystem::path path)
    {
        try {
            boost::filesystem::ifstream file(path);
            file.imbue(std::locale());

            boost::property_tree::ptree pt;
            boost::property_tree::read_json(file, pt);

            key_collection keys;

            // property names map for switch(property_name) construct
            enum pn : int { invalid, name, id, val, data };
            std::unordered_map<std::string, int> pnmap_ = {
                { "name", pn::name }, { "id", pn::id },
                { "val", pn::val }, { "data", pn::data } };
            auto pnmap = [&](std::string const& key) -> int {
                auto res = pnmap_.find(key);
                return (res == pnmap_.end()) ? (pn::invalid) : (res->second);
            };

            for (auto& prop : pt) {
                if (prop.first != "keys") {
                    throw std::runtime_error("invalid property found in root");
                }
                for (auto& li : prop.second) {
                    key_entry key;
                    bool id_set = false, data_set = false, val_set = false;
                    int key_id = 0;
                    for (auto& map_prop : li.second) {
                        switch (pnmap(map_prop.first)) {
                        case pn::name:
                            break;
                        case pn::val:
                            key.val = map_prop.second.get_value<uint32_t>();
                            val_set = true;
                            break;
                        case pn::id:
                            key_id = map_prop.second.get_value<int>();
                            id_set = true;
                            break;
                        case pn::data:
                        {
                            std::vector<uint8_t> key_data;
                            boost::algorithm::unhex(
                                map_prop.second.get_value<std::string>(),
                                std::back_inserter(key_data));
                            if (key_data.size() != key_size) {
                                throw std::runtime_error("key with invalid size found");
                            }
                            std::copy(key_data.begin(), key_data.end(), key.data.begin());
                            data_set = true;
                            break;
                        }
                        default:
                            throw std::runtime_error("invalid property found in key entry");
                        }
                    }
                    if (!(id_set && data_set && val_set)) {
                        throw std::runtime_error("key entry with missing properties found");
                    }
                    keys.insert({ key_id, key });
                }
            }
            return keys;
        }
        catch (std::exception const& e) {
            BOOST_LOG_TRIVIAL(error) <<
                "Error parsing keys: " << e.what();
        }
        return key_collection();
    }

} }  // namespace selink::provider

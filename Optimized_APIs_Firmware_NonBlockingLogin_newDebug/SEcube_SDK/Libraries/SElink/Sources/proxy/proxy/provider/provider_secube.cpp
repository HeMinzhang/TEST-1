/**
 *  \file provider_secube.cpp
 *  \author Nicola Ferri
 *  \brief SEcube provider implementation
 */

#include "proxy/provider/provider_secube.h"

#include <ctime>
#include <algorithm>

#include <boost/log/trivial.hpp>
#include <boost/algorithm/hex.hpp>

namespace selink {
	namespace provider {

		session_secube::session_secube(se3_session* cube, int32_t session_id, session::direction dir)
			: session(dir), session_id_(session_id)
		{
			memcpy(&cube_, cube, sizeof(se3_session));
		}

		session_secube::~session_secube()
		{
			uint8_t tmp[32];
			uint16_t outlen = 0;
            uint16_t r;
			
            r = L1_crypto_update(
				&cube_,
				session_id_, SE3_CRYPTO_FLAG_FINIT,
				0, NULL, 0, NULL, &outlen, tmp);
            if(SE3_OK != r){
                BOOST_LOG_TRIVIAL(trace) << "secube/~ finit failed";
            }
		}

		session::error session_secube::update(size_t nblocks,
			const uint8_t* data_in, uint8_t* data_out)
		{
			const size_t chunksize = SE3_CRYPTO_MAX_DATAIN - 4 * session::block_size;
			size_t nchunk = (nblocks*session::block_size / chunksize);
			uint16_t data_out_len = 0;
			size_t pos = 0;
			int r;
			for (size_t i = 0; i < nchunk; i++) {
				data_out_len = chunksize;

				r = L1_crypto_update(
					&cube_,
					session_id_, 0,
					0, NULL,
					(uint16_t)chunksize, (uint8_t*)data_in + pos,
					&data_out_len,
					data_out + pos);

				if (r != SE3_OK || data_out_len != chunksize) {
                    BOOST_LOG_TRIVIAL(error) << "secube/update error";
					return session::error::provider;
				}
				pos += chunksize;
			}
			size_t remaining = (nblocks*session::block_size) % chunksize;
			if (remaining > 0) {
				data_out_len = (uint16_t)remaining;

				r = L1_crypto_update(
					&cube_,
					session_id_, 0,
					0, NULL,
					(uint16_t)remaining, (uint8_t*)data_in + pos,
					&data_out_len,
					data_out + pos);

				if (r != SE3_OK || data_out_len != remaining) {
                    BOOST_LOG_TRIVIAL(error) << "secube/update error";
					return session::error::provider;
				}
			}

			return session::error::ok;
		}

		session::error session_secube::finalize(uint8_t* auth)
		{
			uint16_t data_out_len = 32;

			int r = L1_crypto_update(
				&cube_,
				session_id_, SE3_CRYPTO_FLAG_AUTH,
				0, NULL, 0, NULL, &data_out_len, auth_);

			if (r != SE3_OK || data_out_len != 32) {
                BOOST_LOG_TRIVIAL(error) << "secube/finalize error";
				return session::error::provider;
			}
			memcpy(auth, auth_ + 16, session::auth_size);
			return session::error::ok;
		}


		session::error session_secube::reset(const uint8_t* iv)
		{
			memcpy(&iv_[0], iv, session::iv_size);

			uint16_t data_out_len = 0;

			int r = L1_crypto_update(
				&cube_,
				session_id_, SE3_CRYPTO_FLAG_RESET,
				session::iv_size, &iv_[0],
				0, NULL,
				&data_out_len, NULL);

			if (r != SE3_OK) {
                BOOST_LOG_TRIVIAL(error) << "secube/reset error";
				return session::error::provider;
			}
			return session::error::ok;
		}

		provider_secube::provider_secube()
			: provider(), status_(wait_config)
		{
			memset(&cube, 0, sizeof(se3_session));
		}

		provider_secube::~provider_secube()
		{
			if (status_ == logged_in) {
				L1_logout(&cube);
				L0_close((&cube.device));
			}
		}

		void provider_secube::config(
			std::array<uint8_t, provider::sn_size> dsn,
			std::vector<uint8_t> pin)
		{
			boost::mutex::scoped_lock lock(config_mut);

			if (status_ == logged_in) {
				L1_logout(&cube);
				L0_close(&cube.device);
			}
			status_ = wait_config;

			if (pin.size() > SE3_L1_PIN_SIZE) {
				return;
			}
			std::copy(dsn.begin(), dsn.end(), sn_.begin());
			std::fill(pin_.begin(), pin_.end(), 0);
			std::copy(pin.begin(), pin.end(), pin_.begin());

			status_ = error_notfound;
		}

		int provider_secube::status()
		{
			return status_;
		}


		void provider_secube::login()
		{
			if (status_ != logged_in) {
				_login();
			}
		}


		std::vector<provider::device> provider_secube::discover()
		{
			se3_disco_it it;
			std::vector<provider::device> ret;
			L0_discover_init(&it);
			while (L0_discover_next(&it)) {
				provider::device tmp;
				tmp.root = boost::filesystem::path(it.device_info.path);
				memcpy(&(tmp.sn[0]), it.device_info.serialno, provider::sn_size);
				ret.push_back(tmp);
			}
			return ret;
		}

		void provider_secube::reset()
		{
			ref++;
			if (status_ == logged_in) {
				L1_logout(&cube);
				L0_close(&(cube.device));
			}

			pin_.fill(0);
			sn_.fill(0);

			status_ = wait_config;
		}

		bool provider_secube::_login()
		{
			bool success = false;

			std::vector<uint8_t> pin_copy;
			std::array<uint8_t, sn_size> sn_copy;
			{
				boost::mutex::scoped_lock lock(config_mut);
				std::copy(sn_.begin(), sn_.end(), sn_copy.begin());
				pin_copy.resize(pin_.size());
				std::copy(pin_.begin(), pin_.end(), pin_copy.begin());
			}

			do {
				if (status_ == logged_in) {
					L1_logout(&cube);
					L0_close(&cube.device);

				}

				se3_disco_it it;
				bool found = false;
				L0_discover_init(&it);
				while (L0_discover_next(&it)) {
                    std::string tmp;
                    boost::algorithm::hex(it.device_info.serialno,
                        it.device_info.serialno+provider::sn_size, 
                        std::back_inserter(tmp));
                    BOOST_LOG_TRIVIAL(trace) << "secube/login DEV " << tmp;
					if (!memcmp(it.device_info.serialno, &sn_copy[0], provider::sn_size)) {
						if (it.device_info.status == 0) {
							found = true;
                            BOOST_LOG_TRIVIAL(trace) << "secube/login Device Found";
							break;
						}
					}
				}
				if (!found) {
					status_ = error_notfound;
					break;
				}
				se3_device cube_dev;

				if (SE3_OK != L0_open(&cube_dev, &it.device_info, 1000)) {
					status_ = error_device;
                    BOOST_LOG_TRIVIAL(error) << "secube/login error opening device";
					break;
				}

				uint16_t r = L1_login(&cube, &cube_dev, &pin_copy[0], SE3_ACCESS_USER);
				if (r == SE3_OK) {
					auto unix_time = boost::chrono::duration_cast<boost::chrono::seconds>(
						boost::chrono::system_clock::now().time_since_epoch()).count();
					L1_crypto_set_time(&cube, (uint32_t)unix_time);

					status_ = logged_in;
                    BOOST_LOG_TRIVIAL(trace) << "secube/login logged in successfully";
					success = true;
					break;
				}
				else if (r == SE3_ERR_PIN) {
					status_ = error_userpin;
                    BOOST_LOG_TRIVIAL(trace) << "secube/login error logging in (ERR_PIN)";
					L0_close(&cube_dev);
					break;
				}
				else {
					status_ = error_device;
                    BOOST_LOG_TRIVIAL(error) << "secube/login error logging in (" << r << ")";
					L0_close(&cube_dev);
					break;
				}
			} while (false);

			std::fill(pin_copy.begin(), pin_copy.end(), 0);
			return success;
		}

		std::shared_ptr<session> provider_secube::create_session(
			int id, const uint8_t* nonce, session::direction dir)
		{
			uint32_t session_id;
			uint16_t algo_mode =
				(dir == session::direction::decrypt) ?
				(SE3_DIR_DECRYPT) : (SE3_DIR_ENCRYPT);
			algo_mode |= SE3_FEEDBACK_CBC;
			int r;

			BOOST_LOG_TRIVIAL(trace) <<
				"nonce " << (unsigned)nonce[0] << " " << (unsigned)nonce[1] <<
				" " << ((dir == session::direction::encrypt) ? "encrypt" : "decrypt");

			auto op_crypto_init = [&]() -> int {
				int cr;
				cr = L1_crypto_init(&cube, SE3_ALGO_AES_HMACSHA256, algo_mode, id, &session_id);
				if (cr != SE3_OK) {
					return cr;
				}
				uint16_t data_out_len = 0;
				cr = L1_crypto_update(&cube, session_id, SE3_CRYPTO_FLAG_SETNONCE, provider::nonce_size, (uint8_t*)nonce, 0, NULL, &data_out_len, NULL);
				return cr;
			};

			r = SE3_ERR_COMM;
			if (status_ == logged_in) {
				r = op_crypto_init();
			}


			if (r == SE3_OK) {
				return std::shared_ptr<session>(new session_secube(&cube, session_id, dir));
			}
			else if (r == SE3_ERR_MEMORY) {
                BOOST_LOG_TRIVIAL(trace) << "secube/create_session insufficient memory on device";
				return std::shared_ptr<session>(nullptr);
			}
			else {
                BOOST_LOG_TRIVIAL(trace) << "secube/create_session connection with device lost. retrying login";
				bool b = _login();
				if (b) {
					r = op_crypto_init();
				}
			}
			if (r == SE3_OK) {
				return std::shared_ptr<session>(new session_secube(&cube, session_id, dir));
			}
			return std::shared_ptr<session>(nullptr);
		}
	}
}  // namespace selink::provider

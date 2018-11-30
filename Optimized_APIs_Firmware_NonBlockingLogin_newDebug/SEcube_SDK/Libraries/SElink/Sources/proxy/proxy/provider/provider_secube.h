/**
 *  \file provider_secube.h
 *  \author Nicola Ferri
 *  \brief SEcube provider implementation
 */

#pragma once
#include "util/os.h"

#include <memory>
#include <vector>
#include <string>

#include <boost/thread.hpp>

#include "secube/aes256.h"
#include "secube/sha256.h"
#include "secube/L1.h"

#include "proxy/provider/provider.h"

namespace selink {
	namespace provider {

		class session_secube : public session {
		public:
			session_secube(se3_session* cube, int32_t session_id, session::direction dir);
			~session_secube() override;
			session::error update(size_t nblocks,
				const uint8_t * data_in, uint8_t * data_out) override;
			session::error finalize(uint8_t* auth) override;
			session::error reset(const uint8_t * iv) override;

		private:
			uint8_t auth_[32];
			uint8_t iv_[session::iv_size];
			int direction_;
			int32_t session_id_;
			se3_session cube_;
		};


		class provider_secube : public provider {
		public:
			provider_secube();
			~provider_secube() override;

			int status() override;
			void config(std::array<uint8_t, provider::sn_size> sn,
				std::vector<uint8_t> pin) override;
			void login() override;

			std::vector<provider::device> discover() override;
			void reset() override;


			std::shared_ptr<session> create_session(
				int id, const uint8_t* nonce, session::direction dir) override;

		private:
			bool _login();

			boost::mutex config_mut;
			boost::mutex keys_mut;
			std::array<uint8_t, provider::sn_size> sn_;
			std::array<uint8_t, SE3_L1_PIN_SIZE> pin_;
			int status_;
			se3_session cube;
		};

	}
}  // namespace selink::provider


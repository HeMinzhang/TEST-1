/**
 *  \file main.cpp
 *  \author Nicola Ferri
 *  \brief Import keys to SEcube device
 *
 *  Import keys from a json file to a SEcube device
 */

#include "util/os.h"

#include <array>

#include <boost/program_options.hpp>

#include "secube/L1.h"
#include "proxy/provider/provider_soft.h"

using namespace selink;

std::string getpin() {
    std::string userpin;
    std::cout << "Pin: " << std::endl;
    std::cin >> userpin;
    if (userpin.length() > provider::provider::pin_max) {
        return std::string("");
    }
    return userpin;
}


bool secube_import_keys(provider::provider_soft::key_collection& keys)
{
    se3_disco_it it;
	se3_device dev;
    se3_session s;
    bool success = true;
    bool opened = false;
    bool logged_in = false;

    L0_discover_init(&it);
    while (L0_discover_next(&it)) {
        if (it.device_info.status == 0) {
            if (SE3_OK != L0_open(&dev, &(it.device_info), 500))
                break;
            opened = true;
			int r;

#if 0
            uint8_t pin0[32] = { 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0 };
            uint8_t ciao[32] = { 'c','i','a','o', 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0 };
            r = L1_login(&s, &dev, pin0, SE3_ACCESS_ADMIN);
			r = L1_set_user_PIN(&s, ciao);
			r = L1_logout(&s);
#endif
            std::string userpin = getpin();
            std::array<uint8_t, SE3_L1_PIN_SIZE> pin;
            std::fill(pin.begin(), pin.end(), 0);
            std::copy(userpin.begin(), userpin.end(), pin.begin());
            r = L1_login(&s, &dev, &pin[0], SE3_ACCESS_USER);
            if (r != SE3_OK) {
                success = false;
                break;
            }
            logged_in = true;
            for (auto nk : keys) {
                std::stringstream ss;
                ss << "key_" << nk.first;
                std::string key_name = ss.str();

                se3_key key;
				memset(key.name, 0, SE3_KEY_NAME_MAX);
				strcpy((char*)key.name, key_name.c_str());
                key.name_size = (uint16_t)key_name.length();
                key.data = &(nk.second.data[0]);
                key.data_size = (uint16_t)nk.second.data.size();
                key.id = (uint32_t)nk.first;
                key.validity = nk.second.val;

                r = L1_key_edit(&s, SE3_KEY_OP_UPSERT, &key);
                if (r != SE3_OK) {
                    success = false;
                    break;
                }
            }
            break;
        }
    }
    if (logged_in) {
        L1_logout(&s);
    }
    if (opened) {
        L0_close(&s.device);
    }

    return success;
}



int import_keys()
{
    std::locale::global(std::locale(""));
    auto argv = boost::program_options::split_winmain(GetCommandLineW());
    if (argv.size() != 2) {
        std::cout << "USAGE: secube_import_keys keys.json" << std::endl;
    }

    std::wcout << L"Importing from " << argv[1] << std::endl;
    provider::provider_soft::key_collection keys =
        provider::provider_soft::keys_from_file(argv[1]);

    if (keys.empty()) {
        std::cout << "No keys to insert" << std::endl;
        return 0;
    }
    bool success = false;


    success = secube_import_keys(keys);


    if (!success) {
        std::cout << "Error" << std::endl;
        return 1;
    }
    std::cout << "Success" << std::endl;
    return 0;
}


int main() {
    return import_keys();
}


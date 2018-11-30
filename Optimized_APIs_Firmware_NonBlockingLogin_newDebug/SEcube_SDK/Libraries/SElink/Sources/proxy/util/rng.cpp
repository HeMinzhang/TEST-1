/**
 *  \file rng.h
 *  \author Nicola Ferri
 *  \brief Cross-platform random number generation utilities
 */

#include "util/rng.h"

#ifdef _WIN32
#include <wincrypt.h>
#endif  // _WIN32

namespace selink { namespace util { namespace rng {

    void rand_bytes(uint8_t* dst, size_t dst_size)
    {
#ifdef _WIN32
        HCRYPTPROV hProvider;
        if (FALSE == CryptAcquireContextW(&hProvider, NULL, NULL, PROV_RSA_FULL, 0)) {
            if (NTE_BAD_KEYSET == GetLastError()) {
                if (FALSE == CryptAcquireContextW(
                    &hProvider, NULL, NULL, PROV_RSA_FULL, CRYPT_NEWKEYSET))
                {
                    hProvider = NULL;
                }
            }
        }
        if (hProvider != NULL) {
            CryptGenRandom(hProvider, (DWORD)dst_size, dst);
            CryptReleaseContext(hProvider, 0U);
        }
        else {
            for (size_t i = 0; i < dst_size; i++) {
                dst[i] = (uint8_t)rand();
            }
        }
#elif defined(__linux__)
        int frnd = open("/dev/urandom", O_RDONLY);
        read(frnd, dst, dst_size);
        close(frnd);
#endif  // _WIN32
    }

} } }  // namespace selink::util::rng

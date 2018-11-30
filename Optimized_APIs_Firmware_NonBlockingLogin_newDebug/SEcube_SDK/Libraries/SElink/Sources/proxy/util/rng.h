/**
 *  \file rng.h
 *  \author Nicola Ferri
 *  \brief Cross-platform random number generation utilities
 */

#include "util/os.h"
#include <cstdlib>
#include <cstdint>

namespace selink { namespace util { namespace rng {

    /** \brief Generate random bytes
     *  \param dst destination vector
     *  \param dst_size size of the destination vector in bytes
     *  
     *  Uses /dev/urandom on Linux or CryptGenRandom on Windows
     */
    void rand_bytes(uint8_t* dst, size_t dst_size);

} } }  // namespace selink::util::rng


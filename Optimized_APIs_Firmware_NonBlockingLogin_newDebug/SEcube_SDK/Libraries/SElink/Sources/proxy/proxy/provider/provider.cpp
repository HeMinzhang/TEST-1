/**
 *  \file provider.cpp
 *  \author Nicola Ferri
 *  \brief provider and session interfaces
 */

#include "proxy/provider/provider.h"

#include <algorithm>

namespace selink { namespace provider {

    session::session(direction dir)
        : direction_(dir)
    { }

    session::~session()
    { }

    size_t session::padded_size(size_t size)
    {
        size_t m = size % block_size;
        if (m == 0) {
            return size;
        }
        return size + (block_size - m);
    }

    provider::provider()
    { }

    provider::~provider()
    { }

    const uint8_t session::test_key[32] = {
        0x85, 0xfa, 0x93, 0x01, 0x67, 0x41, 0xb2, 0x18,
        0xcc, 0xe6, 0xc4, 0x53, 0xa3, 0x26, 0x74, 0x22,
        0x1d, 0x8c, 0x7a, 0x9e, 0x1f, 0x00, 0x98, 0x26,
        0x7e, 0xca, 0x06, 0x87, 0x9d, 0x95, 0xa9, 0xed };
    const uint8_t session::test_iv[16] = {
        0x90, 0xc5, 0xd2, 0xf3, 0x31, 0xd4, 0x6f, 0x6d,
        0x1b, 0x45, 0xc1, 0xa0, 0x88, 0x56, 0x5b, 0x4c };
    const uint8_t provider::test_nonce[32] = {
        0x85, 0xfa, 0x93, 0x01, 0x67, 0x41, 0xb2, 0x18,
        0xcc, 0xe6, 0xc4, 0x53, 0xa3, 0x26, 0x74, 0x22,
        0x1d, 0x8c, 0x7a, 0x9e, 0x1f, 0x00, 0x98, 0x26,
        0x7e, 0xca, 0x06, 0x87, 0x9d, 0x95, 0xa9, 0xed };

} }

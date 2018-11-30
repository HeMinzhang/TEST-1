#pragma once

#include <stddef.h>
#include <stdint.h>


#ifdef __cplusplus
extern "C" {
#endif

	extern const uint16_t se3_crc16_table[0x100];
	uint16_t se3_crc16_update(size_t length, const uint8_t* data, uint16_t crc);

#ifdef __cplusplus
}
#endif

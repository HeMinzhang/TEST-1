#pragma once

#include "se3_common.h"
#include "se3comm.h"
#include "crc16.h"


#ifdef __cplusplus
extern "C" {
#endif

typedef struct se3_device_info_ {
    se3_char path[SE3_MAX_PATH];
    uint8_t serialno[SE3_SN_SIZE];
    uint8_t hello_msg[SE3_HELLO_SIZE];
    uint16_t status;
} se3_device_info;

typedef struct se3_device_ {
    se3_device_info info;
    uint8_t* request;
    uint8_t* response;
	se3_file f;
    bool opened;
} se3_device;

typedef struct se3_disco_it_ {
    se3_device_info device_info;
    se3_drive_it _drive_it;
} se3_disco_it;

#ifdef CUBESIM
uint16_t L0_open_sim(se3_device* s);
#endif


/* defines */
#define SE3_NBLOCKS    (SE3_COMM_N-1)   // Number of blocks
#ifdef CUBESIM
#define SE3_TIMEOUT (1024*1024)
#else
#define SE3_TIMEOUT   (10000)
#endif

/* Sizes (in Bytes) */

#define SE3_RES_SIZE_HEADER   (32)

#define SE3_SIZE_PAYLOAD_MAX   ((SE3_COMM_BLOCK * SE3_NBLOCKS) - SE3_REQ_SIZE_HEADER - (SE3_COMM_BLOCK * SE3_REQDATA_SIZE_HEADER))
/* */


uint16_t L0_TXRX(se3_device* device, uint16_t req_cmd, uint16_t req_cmdflags, uint16_t req_len, const uint8_t* req_data, uint16_t* resp_status, uint16_t* resp_len, uint8_t* resp_data);

uint16_t L0_echo(se3_device* device, const uint8_t* data_in, uint16_t data_in_len, uint8_t* data_out);
uint16_t L0_factoryinit(se3_device* device, const uint8_t* serialno);
uint16_t L0_open(se3_device* dev, se3_device_info* dev_info, uint32_t timeout);
void L0_close(se3_device* dev);

bool L0_discover_serialno(uint8_t* serialno, se3_device_info* device);
void L0_discover_init(se3_disco_it* it);
bool L0_discover_next(se3_disco_it* it);

#ifdef __cplusplus
}
#endif

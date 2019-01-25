#ifndef FPGAIPM_H_
#define FPGAIMP_H_

#include "stm32f4xx_hal.h"
#include "stm32f4xx_hal_sram.h"
#include "stm32f4xx.h"


// class types
#define FPGA_IPM_UINT8   uint8_t
#define FPGA_IPM_UINT16  uint16_t
#define FPGA_IPM_BOOLEAN int
#define FPGA_IPM_SEM     FPGA_IPM_BOOLEAN

// specific types
#define FPGA_IPM_CORE    FPGA_IPM_UINT8
#define FPGA_IPM_ADDRESS FPGA_IPM_UINT8
#define FPGA_IPM_DATA    FPGA_IPM_UINT16
#define FPGA_IPM_OPCODE  FPGA_IPM_UINT8


// masks
#define FPGA_IPM_CORE_MASK    0b1111111u
#define FPGA_IPM_ADDRESS_MASK 0b111111u
#define FPGA_IPM_OPCODE_MASK  0b111111u

// offsets
//#define FPGA_IPM_CORE_OFFSET 9 ??
#define FPGA_IPM_OPCODE_OFFSET 10

// constants
#define FPGA_IPM_BEGIN_TRANSACTION 0b0000000010000000u
#define FPGA_IPM_INTERRUPT_MODE    0b0000001000000000u
#define FPGA_IPM_ACK 			   0b0000000100000000u
#define FPGA_IPM_SRAM_BASE_ADDR    0x60000000U


FPGA_IPM_BOOLEAN FPGA_IPM_init(void);
FPGA_IPM_UINT8 FPGA_IPM_read(FPGA_IPM_CORE coreID, FPGA_IPM_ADDRESS address, FPGA_IPM_DATA *dataPtr);
FPGA_IPM_UINT8 FPGA_IPM_write(FPGA_IPM_CORE coreID, FPGA_IPM_ADDRESS address, FPGA_IPM_DATA *dataPtr);
FPGA_IPM_BOOLEAN FPGA_IPM_open(FPGA_IPM_CORE coreID, FPGA_IPM_OPCODE opcode, FPGA_IPM_BOOLEAN interruptMode, FPGA_IPM_BOOLEAN ack);
FPGA_IPM_BOOLEAN FPGA_IPM_close(FPGA_IPM_CORE coreID);

#endif


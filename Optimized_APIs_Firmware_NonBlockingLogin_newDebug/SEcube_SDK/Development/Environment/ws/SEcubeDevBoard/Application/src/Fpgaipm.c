#include "Fpgaipm.h"

#include "stm32f4xx.h"
#include "stm32f4xx_hal.h"
#include "stm32f4xx_hal_sram.h"
#include "stm32f4xx_exti.h"
#include "stm32f4xx_syscfg.h"
#include "misc.h"

static FPGA_IPM_DATA row0;
static FPGA_IPM_CORE currentCore;
static FPGA_IPM_BOOLEAN initialized;
static FPGA_IPM_SEM sem;
static SRAM_HandleTypeDef SRAM_LETTURA;
static SRAM_HandleTypeDef SRAM_SCRITTURA;
static FPGA_IPM_BOOLEAN checkCore(FPGA_IPM_CORE coreID);
static void writeRow0(FPGA_IPM_DATA newRow0);
static void readRow0();

static FPGA_IPM_BOOLEAN initialized = 0;

FPGA_IPM_BOOLEAN FPGA_IPM_init() {
    
  if (initialized) return 0;
  initialized = 1;
    
  // CONFIGURE FMC -----------------------------------------------------------------

  		  // INIT FMC
  		  FMC_NORSRAM_TimingTypeDef Timing;
  		  FMC_NORSRAM_TimingTypeDef ExtTiming;

  		  /** Perform the FPGA CS1 memory initialization sequence
  		   */
  		  SRAM_LETTURA.Instance = FMC_NORSRAM_DEVICE;
  		  SRAM_LETTURA.Extended = FMC_NORSRAM_EXTENDED_DEVICE;

  		  /* hsram1.Init */
  		  SRAM_LETTURA.Init.NSBank = FMC_NORSRAM_BANK1;
  		  SRAM_LETTURA.Init.DataAddressMux = FMC_DATA_ADDRESS_MUX_DISABLE;
  		  SRAM_LETTURA.Init.MemoryType = FMC_MEMORY_TYPE_SRAM;
  		  SRAM_LETTURA.Init.MemoryDataWidth = FMC_NORSRAM_MEM_BUS_WIDTH_16;
  		  SRAM_LETTURA.Init.BurstAccessMode = FMC_BURST_ACCESS_MODE_DISABLE;
  		  SRAM_LETTURA.Init.WaitSignalPolarity = FMC_WAIT_SIGNAL_POLARITY_LOW;
  		  SRAM_LETTURA.Init.WrapMode = FMC_WRAP_MODE_DISABLE;
  		  SRAM_LETTURA.Init.WaitSignalActive = FMC_WAIT_TIMING_BEFORE_WS;
  		  SRAM_LETTURA.Init.WriteOperation = FMC_WRITE_OPERATION_ENABLE;
  		  SRAM_LETTURA.Init.WaitSignal = FMC_WAIT_SIGNAL_DISABLE;
  		  SRAM_LETTURA.Init.ExtendedMode = FMC_EXTENDED_MODE_ENABLE;
  		  SRAM_LETTURA.Init.AsynchronousWait = FMC_ASYNCHRONOUS_WAIT_DISABLE;
  		  SRAM_LETTURA.Init.WriteBurst = FMC_WRITE_BURST_DISABLE;
  		  SRAM_LETTURA.Init.ContinuousClock = FMC_CONTINUOUS_CLOCK_SYNC_ONLY;

  		  // Timing (Read)
  		  Timing.AccessMode = FMC_ACCESS_MODE_A;
  		  Timing.AddressSetupTime = 15;
  		  Timing.AddressHoldTime = 0;
  		  Timing.DataSetupTime = 15;
  		  Timing.BusTurnAroundDuration = 0;
  		  Timing.CLKDivision = 10;
  		  Timing.DataLatency = 2;

  		  // ExtTiming (Write)
  		  ExtTiming.AccessMode = FMC_ACCESS_MODE_A;
  		  ExtTiming.AddressSetupTime = 15;
  		  ExtTiming.AddressHoldTime = 0;
  		  ExtTiming.DataSetupTime = 15;
  		  ExtTiming.BusTurnAroundDuration = 0;
  		  ExtTiming.CLKDivision = 10;
  		  ExtTiming.DataLatency = 2;

  		  /** Perform the FPGA CS2 memory initialization sequence
  		   */
  		  SRAM_SCRITTURA.Instance = FMC_NORSRAM_DEVICE;
  		  SRAM_SCRITTURA.Extended = FMC_NORSRAM_EXTENDED_DEVICE;

  		  /* hsram2.Init */
  		  SRAM_SCRITTURA.Init.NSBank = FMC_NORSRAM_BANK2;
  		  SRAM_SCRITTURA.Init.DataAddressMux = FMC_DATA_ADDRESS_MUX_DISABLE;
  		  SRAM_SCRITTURA.Init.MemoryType = FMC_MEMORY_TYPE_SRAM;
  		  SRAM_SCRITTURA.Init.MemoryDataWidth = FMC_NORSRAM_MEM_BUS_WIDTH_16;
  		  SRAM_SCRITTURA.Init.BurstAccessMode = FMC_BURST_ACCESS_MODE_DISABLE;
  		  SRAM_SCRITTURA.Init.WaitSignalPolarity = FMC_WAIT_SIGNAL_POLARITY_LOW;
  		  SRAM_SCRITTURA.Init.WrapMode = FMC_WRAP_MODE_DISABLE;
  		  SRAM_SCRITTURA.Init.WaitSignalActive = FMC_WAIT_TIMING_BEFORE_WS;
  		  SRAM_SCRITTURA.Init.WriteOperation = FMC_WRITE_OPERATION_DISABLE;
  		  SRAM_SCRITTURA.Init.WaitSignal = FMC_WAIT_SIGNAL_DISABLE;
  		  SRAM_SCRITTURA.Init.ExtendedMode = FMC_EXTENDED_MODE_ENABLE;
  		  SRAM_SCRITTURA.Init.AsynchronousWait = FMC_ASYNCHRONOUS_WAIT_DISABLE;
  		  SRAM_SCRITTURA.Init.WriteBurst = FMC_WRITE_BURST_DISABLE;
  		  SRAM_SCRITTURA.Init.ContinuousClock = FMC_CONTINUOUS_CLOCK_SYNC_ONLY;


  	//	  HAL_SRAM_Init(,,);

  		  HAL_SRAM_Init(&SRAM_LETTURA, &Timing, &ExtTiming);
  		  HAL_SRAM_Init(&SRAM_SCRITTURA, &Timing, &ExtTiming);

  		  // INIT GPIO
  		  GPIO_InitTypeDef GPIO_InitStruct;

  		  /* Peripheral clock enable */
  		  __FMC_CLK_ENABLE();

  		  __GPIOF_CLK_ENABLE();
  		  __GPIOE_CLK_ENABLE();
  		  __GPIOD_CLK_ENABLE();
  		  __GPIOG_CLK_ENABLE();

  		  /** FMC GPIO Configuration
  		  PF0   ------> FMC_A0
  		  PF1   ------> FMC_A1
  		  PF2   ------> FMC_A2
  		  PF3   ------> FMC_A3
  		  PF4   ------> FMC_A4
  		  PF5   ------> FMC_A5
  		  PE7   ------> FMC_D4
  		  PE8   ------> FMC_D5
  		  PE9   ------> FMC_D6
  		  PE10   ------> FMC_D7
  		  PE11   ------> FMC_D8
  		  PE12   ------> FMC_D9
  		  PE13   ------> FMC_D10
  		  PE14   ------> FMC_D11
  		  PE15   ------> FMC_D12
  		  PD8   ------> FMC_D13
  		  PD9   ------> FMC_D14
  		  PD10   ------> FMC_D15
  		  PD14   ------> FMC_D0
  		  PD15   ------> FMC_D1
  		  PD0   ------> FMC_D2
  		  PD1   ------> FMC_D3
  		  PD4   ------> FMC_NOE
  		  PD5   ------> FMC_NWE
  		  PD6   ------> FMC_NWAIT
  		  PD7   ------> FMC_NE1
  		  PG9   ------> FMC_NE2
  		  */

  		  GPIO_InitStruct.Pin = GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3 | GPIO_PIN_4 | GPIO_PIN_5;
  		  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
  		  GPIO_InitStruct.Pull = GPIO_NOPULL;
  		  GPIO_InitStruct.Speed = GPIO_SPEED_FAST;
  		  GPIO_InitStruct.Alternate = GPIO_AF12_FMC;
  		  HAL_GPIO_Init(GPIOF, &GPIO_InitStruct);

  		  GPIO_InitStruct.Pin = GPIO_PIN_7 | GPIO_PIN_8 | GPIO_PIN_9 | GPIO_PIN_10 | GPIO_PIN_11 | GPIO_PIN_12 | GPIO_PIN_13 | GPIO_PIN_14 | GPIO_PIN_15;
  		  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
  		  GPIO_InitStruct.Pull = GPIO_NOPULL;
  		  GPIO_InitStruct.Speed = GPIO_SPEED_FAST;
  		  GPIO_InitStruct.Alternate = GPIO_AF12_FMC;
  		  HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);

  		  GPIO_InitStruct.Pin =
  		      GPIO_PIN_8 | GPIO_PIN_9 | GPIO_PIN_10 | GPIO_PIN_14 | GPIO_PIN_15 | GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_4 | GPIO_PIN_5 | GPIO_PIN_6 | GPIO_PIN_7;
  		  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
  		  GPIO_InitStruct.Pull = GPIO_NOPULL;
  		  GPIO_InitStruct.Speed = GPIO_SPEED_FAST;
  		  GPIO_InitStruct.Alternate = GPIO_AF12_FMC;
  		  HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);

  		  GPIO_InitStruct.Pin = GPIO_PIN_9;
  		  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
  		  GPIO_InitStruct.Pull = GPIO_NOPULL;
  		  GPIO_InitStruct.Speed = GPIO_SPEED_FAST;
  		  GPIO_InitStruct.Alternate = GPIO_AF12_FMC;
  		  HAL_GPIO_Init(GPIOG, &GPIO_InitStruct);

  		  // CONFIGURE RESET ---------------------------------------------------------------

  		  /* Set pin PG2 as reset for the FPGA */
  		  GPIO_InitStruct.Pin = FPGA_RST_Pin;
  		  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  		  GPIO_InitStruct.Pull = GPIO_NOPULL;
  		  GPIO_InitStruct.Speed = GPIO_SPEED_FAST;
  		  HAL_GPIO_Init(GPIOG, &GPIO_InitStruct);


    
  // CONFIGURE OUTPUT CLOCK --------------------------------------------------------

  //  /*Initialize MCO1 output, pin PA8 */
  //  TM_MCOOUTPUT_InitMCO1();

  /* Enable clock for GPIOA */
  __HAL_RCC_GPIOA_CLK_ENABLE();
  /* Enable clock for SYSCFG */
  __HAL_RCC_SYSCFG_CLK_ENABLE();

  /* Set pin PA8 as clock output */
  GPIO_InitStruct.Pin = GPIO_PIN_8;
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FAST;
  GPIO_InitStruct.Alternate = GPIO_AF0_MCO;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
    
//  /* Set MCO1 output = PLLCLK with prescaler 2 = 180MHz / 2 = 90MHz*/
//  TM_MCOOUTPUT_SetOutput1(TM_MCOOUTPUT1_Source_PLLCLK, TM_MCOOUTPUT_Prescaler_2);
  __HAL_RCC_MCO1_CONFIG(RCC_MCO1SOURCE_PLLCLK, RCC_MCODIV_3);

  // INIT SEMAFORO
  sem = 1;
    
    
  // CONFIGURE INTERRUPT -----------------------------------------------------------
    
  /* Set variables used */
  EXTI_InitTypeDef EXTI_InitStruct;
  NVIC_InitTypeDef NVIC_InitStruct;
    
  /* Set pin PA9 as input */
  GPIO_InitStruct.Pin = FPGA_INTN_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FAST;
  HAL_GPIO_Init(FPGA_INTN_GPIO_Port, &GPIO_InitStruct);
    
  /* Tell system that you will use PA9 for EXTI_Line9 */
  SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOA, EXTI_PinSource9);
    
  /* PA9 is connected to EXTI_Line9 */
  EXTI_InitStruct.EXTI_Line = EXTI_Line9;
  /* Enable interrupt */
  EXTI_InitStruct.EXTI_LineCmd = ENABLE;
  /* Interrupt mode */
  EXTI_InitStruct.EXTI_Mode = EXTI_Mode_Interrupt;
  /* Triggers on rising and falling edge */
  EXTI_InitStruct.EXTI_Trigger = EXTI_Trigger_Rising;
  /* Add to EXTI */
  EXTI_Init(&EXTI_InitStruct);
    
  /* Add IRQ vector to NVIC */
  /* PA9 is connected to EXTI_Line9, which has EXTI9_5_IRQn vector */
  NVIC_InitStruct.NVIC_IRQChannel = EXTI9_5_IRQn;
  /* Set priority */
  NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 0x00;
  /* Set sub priority */
  NVIC_InitStruct.NVIC_IRQChannelSubPriority = 0x01;
  /* Enable interrupt */
  NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
  /* Add to NVIC */
  NVIC_Init(&NVIC_InitStruct);
    
    
  return 0;
}




FPGA_IPM_UINT8 FPGA_IPM_read(FPGA_IPM_CORE coreID, FPGA_IPM_ADDRESS address, FPGA_IPM_DATA *dataPtr) {
	HAL_StatusTypeDef status;
	if (checkCore(coreID)) {
		uint32_t addr = (FPGA_IPM_SRAM_BASE_ADDR + 2*address);
		status = HAL_SRAM_Read_16b(&SRAM_LETTURA, (uint32_t*)addr, dataPtr, 1);
		return status == HAL_OK ? 0 : 1;
	}
	return 1;
}




FPGA_IPM_UINT8 FPGA_IPM_write(FPGA_IPM_CORE coreID, FPGA_IPM_ADDRESS address, FPGA_IPM_DATA *dataPtr) {
	HAL_StatusTypeDef status;
	if (checkCore(coreID)) {
		uint32_t addr = (FPGA_IPM_SRAM_BASE_ADDR + 2*address);
		status = HAL_SRAM_Write_16b(&SRAM_SCRITTURA, (uint32_t*)addr, dataPtr, 1);
		return status == HAL_OK ? 0 : 1;
	}
	return 1;
}




FPGA_IPM_BOOLEAN FPGA_IPM_open(FPGA_IPM_CORE coreID, FPGA_IPM_OPCODE opcode, FPGA_IPM_BOOLEAN interruptMode, FPGA_IPM_BOOLEAN ack) {
  FPGA_IPM_DATA newRow0;
  if (sem > 0) {
    sem--;
    newRow0 = 0 | (coreID & FPGA_IPM_CORE_MASK) | ((opcode & FPGA_IPM_OPCODE_MASK) << FPGA_IPM_OPCODE_OFFSET) |
              (interruptMode ? FPGA_IPM_INTERRUPT_MODE : 0) | (ack ? FPGA_IPM_ACK : 0) | FPGA_IPM_BEGIN_TRANSACTION;
    writeRow0(newRow0);
    currentCore = coreID;
    return 0;
  }
  return 1;
}




FPGA_IPM_BOOLEAN FPGA_IPM_close(FPGA_IPM_CORE coreID) {
  if (checkCore(coreID)) {
    FPGA_IPM_DATA newRow0 = row0 & ~FPGA_IPM_BEGIN_TRANSACTION;
    writeRow0(newRow0);
    sem++;
    return 0;
  }
  return 1;
}



static FPGA_IPM_BOOLEAN checkCore(FPGA_IPM_CORE coreID) { return coreID == currentCore && sem == 0 && initialized; }

static void writeRow0(FPGA_IPM_DATA newRow0) {
  uint32_t* addr = FPGA_IPM_SRAM_BASE_ADDR;
  HAL_SRAM_Write_16b(&SRAM_SCRITTURA, addr, &newRow0, 1);
  row0 = newRow0;
}

static void readRow0() {
	uint32_t* addr = FPGA_IPM_SRAM_BASE_ADDR;
	HAL_SRAM_Read_16b(&SRAM_LETTURA, addr, &row0, 1);
}


void EXTI9_5_IRQHandler(void) {
    int ii;
	/* Make sure that interrupt flag is set */
    if (EXTI_GetITStatus(EXTI_Line9) != RESET) {
        /* Do your stuff when PA9 is changed */
        /* switch among different cores and call the specific ISR for everyone of them */
    	readRow0(); //TODO: è questa chiamata a funzione che scaturisce l'errore (potrebbe essere un problema della funzione exti9_5IRQHandler)
    	switch(row0) {
    		case 1:
    			for(ii=0; ii<50000000; ii++);
    			HAL_GPIO_WritePin(GPIOG, FPGA_RST_Pin, GPIO_PIN_SET);
    			HAL_GPIO_WritePin(GPIOG, FPGA_RST_Pin, GPIO_PIN_RESET);
    			break;
    		default:
    			break;
    	}
        /* Clear interrupt flag */
        EXTI_ClearITPendingBit(EXTI_Line9);
    }
}



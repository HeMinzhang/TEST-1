################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
/Users/Nico/Documents/SeCube_Project/Code/Optimized_APIs_Firmware_NonBlockingLogin_newDebug/SEcube_SDK/Development/Environment/secube/src/adc.c \
/Users/Nico/Documents/SeCube_Project/Code/Optimized_APIs_Firmware_NonBlockingLogin_newDebug/SEcube_SDK/Development/Environment/secube/src/crc.c \
/Users/Nico/Documents/SeCube_Project/Code/Optimized_APIs_Firmware_NonBlockingLogin_newDebug/SEcube_SDK/Development/Environment/secube/src/dma.c \
/Users/Nico/Documents/SeCube_Project/Code/Optimized_APIs_Firmware_NonBlockingLogin_newDebug/SEcube_SDK/Development/Environment/secube/src/fmc.c \
/Users/Nico/Documents/SeCube_Project/Code/Optimized_APIs_Firmware_NonBlockingLogin_newDebug/SEcube_SDK/Development/Environment/secube/src/gpio.c \
/Users/Nico/Documents/SeCube_Project/Code/Optimized_APIs_Firmware_NonBlockingLogin_newDebug/SEcube_SDK/Development/Environment/secube/src/i2c.c \
/Users/Nico/Documents/SeCube_Project/Code/Optimized_APIs_Firmware_NonBlockingLogin_newDebug/SEcube_SDK/Development/Environment/secube/src/main.c \
/Users/Nico/Documents/SeCube_Project/Code/Optimized_APIs_Firmware_NonBlockingLogin_newDebug/SEcube_SDK/Development/Environment/secube/src/rng.c \
/Users/Nico/Documents/SeCube_Project/Code/Optimized_APIs_Firmware_NonBlockingLogin_newDebug/SEcube_SDK/Development/Environment/secube/src/sdio.c \
/Users/Nico/Documents/SeCube_Project/Code/Optimized_APIs_Firmware_NonBlockingLogin_newDebug/SEcube_SDK/Development/Environment/secube/src/se3_rand.c \
/Users/Nico/Documents/SeCube_Project/Code/Optimized_APIs_Firmware_NonBlockingLogin_newDebug/SEcube_SDK/Development/Environment/secube/src/se3_sdio.c \
/Users/Nico/Documents/SeCube_Project/Code/Optimized_APIs_Firmware_NonBlockingLogin_newDebug/SEcube_SDK/Development/Environment/secube/src/spi.c \
/Users/Nico/Documents/SeCube_Project/Code/Optimized_APIs_Firmware_NonBlockingLogin_newDebug/SEcube_SDK/Development/Environment/secube/src/stm32f4xx_hal_msp.c \
/Users/Nico/Documents/SeCube_Project/Code/Optimized_APIs_Firmware_NonBlockingLogin_newDebug/SEcube_SDK/Development/Environment/secube/src/stm32f4xx_it.c \
/Users/Nico/Documents/SeCube_Project/Code/Optimized_APIs_Firmware_NonBlockingLogin_newDebug/SEcube_SDK/Development/Environment/secube/src/tim.c \
/Users/Nico/Documents/SeCube_Project/Code/Optimized_APIs_Firmware_NonBlockingLogin_newDebug/SEcube_SDK/Development/Environment/secube/src/usart.c \
/Users/Nico/Documents/SeCube_Project/Code/Optimized_APIs_Firmware_NonBlockingLogin_newDebug/SEcube_SDK/Development/Environment/secube/src/usb_device.c \
/Users/Nico/Documents/SeCube_Project/Code/Optimized_APIs_Firmware_NonBlockingLogin_newDebug/SEcube_SDK/Development/Environment/secube/src/usbd_conf.c \
/Users/Nico/Documents/SeCube_Project/Code/Optimized_APIs_Firmware_NonBlockingLogin_newDebug/SEcube_SDK/Development/Environment/secube/src/usbd_desc.c \
/Users/Nico/Documents/SeCube_Project/Code/Optimized_APIs_Firmware_NonBlockingLogin_newDebug/SEcube_SDK/Development/Environment/secube/src/usbd_storage_if.c 

OBJS += \
./Application/src/adc.o \
./Application/src/crc.o \
./Application/src/dma.o \
./Application/src/fmc.o \
./Application/src/gpio.o \
./Application/src/i2c.o \
./Application/src/main.o \
./Application/src/rng.o \
./Application/src/sdio.o \
./Application/src/se3_rand.o \
./Application/src/se3_sdio.o \
./Application/src/spi.o \
./Application/src/stm32f4xx_hal_msp.o \
./Application/src/stm32f4xx_it.o \
./Application/src/tim.o \
./Application/src/usart.o \
./Application/src/usb_device.o \
./Application/src/usbd_conf.o \
./Application/src/usbd_desc.o \
./Application/src/usbd_storage_if.o 

C_DEPS += \
./Application/src/adc.d \
./Application/src/crc.d \
./Application/src/dma.d \
./Application/src/fmc.d \
./Application/src/gpio.d \
./Application/src/i2c.d \
./Application/src/main.d \
./Application/src/rng.d \
./Application/src/sdio.d \
./Application/src/se3_rand.d \
./Application/src/se3_sdio.d \
./Application/src/spi.d \
./Application/src/stm32f4xx_hal_msp.d \
./Application/src/stm32f4xx_it.d \
./Application/src/tim.d \
./Application/src/usart.d \
./Application/src/usb_device.d \
./Application/src/usbd_conf.d \
./Application/src/usbd_desc.d \
./Application/src/usbd_storage_if.d 


# Each subdirectory must supply rules for building sources it contributes
Application/src/adc.o: /Users/Nico/Documents/SeCube_Project/Code/Optimized_APIs_Firmware_NonBlockingLogin_newDebug/SEcube_SDK/Development/Environment/secube/src/adc.c
	@echo 'Building file: $<'
	@echo 'Invoking: MCU GCC Compiler'
	@echo $(PWD)
	arm-none-eabi-gcc -mcpu=cortex-m4 -mthumb -mfloat-abi=hard -mfpu=fpv4-sp-d16 -std=c99 '-D__weak=__attribute__((weak))' '-D__packed="__attribute__((__packed__))"' -DUSE_HAL_DRIVER -DSTM32F429xx -I"/Users/Nico/Documents/SeCube_Project/Code/Optimized_APIs_Firmware_NonBlockingLogin_newDebug/SEcube_SDK/Development/Environment/Drivers/STM32F4xx_HAL_Driver/Inc" -I"/Users/Nico/Documents/SeCube_Project/Code/Optimized_APIs_Firmware_NonBlockingLogin_newDebug/SEcube_SDK/Development/Environment/Drivers/STM32F4xx_HAL_Driver/Inc/Legacy" -I"/Users/Nico/Documents/SeCube_Project/Code/Optimized_APIs_Firmware_NonBlockingLogin_newDebug/SEcube_SDK/Development/Environment/Middlewares/ST/STM32_USB_Device_Library/Core/Inc" -I"/Users/Nico/Documents/SeCube_Project/Code/Optimized_APIs_Firmware_NonBlockingLogin_newDebug/SEcube_SDK/Development/Environment/Middlewares/ST/STM32_USB_Device_Library/Class/MSC/Inc" -I"/Users/Nico/Documents/SeCube_Project/Code/Optimized_APIs_Firmware_NonBlockingLogin_newDebug/SEcube_SDK/Development/Environment/Drivers/CMSIS/Include" -I"/Users/Nico/Documents/SeCube_Project/Code/Optimized_APIs_Firmware_NonBlockingLogin_newDebug/SEcube_SDK/Development/Environment/Drivers/CMSIS/Device/ST/STM32F4xx/Include" -I"/Users/Nico/Documents/SeCube_Project/Code/Optimized_APIs_Firmware_NonBlockingLogin_newDebug/SEcube_SDK/Development/Environment/secube/src" -I"/Users/Nico/Documents/SeCube_Project/Code/Optimized_APIs_Firmware_NonBlockingLogin_newDebug/SEcube_SDK/Development/Environment/secube/src/Common" -I"/Users/Nico/Documents/SeCube_Project/Code/Optimized_APIs_Firmware_NonBlockingLogin_newDebug/SEcube_SDK/Development/Environment/secube/src/Device"  -O0 -g3 -Wall -fmessage-length=0 -ffunction-sections -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

Application/src/crc.o: /Users/Nico/Documents/SeCube_Project/Code/Optimized_APIs_Firmware_NonBlockingLogin_newDebug/SEcube_SDK/Development/Environment/secube/src/crc.c
	@echo 'Building file: $<'
	@echo 'Invoking: MCU GCC Compiler'
	@echo $(PWD)
	arm-none-eabi-gcc -mcpu=cortex-m4 -mthumb -mfloat-abi=hard -mfpu=fpv4-sp-d16 -std=c99 '-D__weak=__attribute__((weak))' '-D__packed="__attribute__((__packed__))"' -DUSE_HAL_DRIVER -DSTM32F429xx -I"/Users/Nico/Documents/SeCube_Project/Code/Optimized_APIs_Firmware_NonBlockingLogin_newDebug/SEcube_SDK/Development/Environment/Drivers/STM32F4xx_HAL_Driver/Inc" -I"/Users/Nico/Documents/SeCube_Project/Code/Optimized_APIs_Firmware_NonBlockingLogin_newDebug/SEcube_SDK/Development/Environment/Drivers/STM32F4xx_HAL_Driver/Inc/Legacy" -I"/Users/Nico/Documents/SeCube_Project/Code/Optimized_APIs_Firmware_NonBlockingLogin_newDebug/SEcube_SDK/Development/Environment/Middlewares/ST/STM32_USB_Device_Library/Core/Inc" -I"/Users/Nico/Documents/SeCube_Project/Code/Optimized_APIs_Firmware_NonBlockingLogin_newDebug/SEcube_SDK/Development/Environment/Middlewares/ST/STM32_USB_Device_Library/Class/MSC/Inc" -I"/Users/Nico/Documents/SeCube_Project/Code/Optimized_APIs_Firmware_NonBlockingLogin_newDebug/SEcube_SDK/Development/Environment/Drivers/CMSIS/Include" -I"/Users/Nico/Documents/SeCube_Project/Code/Optimized_APIs_Firmware_NonBlockingLogin_newDebug/SEcube_SDK/Development/Environment/Drivers/CMSIS/Device/ST/STM32F4xx/Include" -I"/Users/Nico/Documents/SeCube_Project/Code/Optimized_APIs_Firmware_NonBlockingLogin_newDebug/SEcube_SDK/Development/Environment/secube/src" -I"/Users/Nico/Documents/SeCube_Project/Code/Optimized_APIs_Firmware_NonBlockingLogin_newDebug/SEcube_SDK/Development/Environment/secube/src/Common" -I"/Users/Nico/Documents/SeCube_Project/Code/Optimized_APIs_Firmware_NonBlockingLogin_newDebug/SEcube_SDK/Development/Environment/secube/src/Device"  -O0 -g3 -Wall -fmessage-length=0 -ffunction-sections -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

Application/src/dma.o: /Users/Nico/Documents/SeCube_Project/Code/Optimized_APIs_Firmware_NonBlockingLogin_newDebug/SEcube_SDK/Development/Environment/secube/src/dma.c
	@echo 'Building file: $<'
	@echo 'Invoking: MCU GCC Compiler'
	@echo $(PWD)
	arm-none-eabi-gcc -mcpu=cortex-m4 -mthumb -mfloat-abi=hard -mfpu=fpv4-sp-d16 -std=c99 '-D__weak=__attribute__((weak))' '-D__packed="__attribute__((__packed__))"' -DUSE_HAL_DRIVER -DSTM32F429xx -I"/Users/Nico/Documents/SeCube_Project/Code/Optimized_APIs_Firmware_NonBlockingLogin_newDebug/SEcube_SDK/Development/Environment/Drivers/STM32F4xx_HAL_Driver/Inc" -I"/Users/Nico/Documents/SeCube_Project/Code/Optimized_APIs_Firmware_NonBlockingLogin_newDebug/SEcube_SDK/Development/Environment/Drivers/STM32F4xx_HAL_Driver/Inc/Legacy" -I"/Users/Nico/Documents/SeCube_Project/Code/Optimized_APIs_Firmware_NonBlockingLogin_newDebug/SEcube_SDK/Development/Environment/Middlewares/ST/STM32_USB_Device_Library/Core/Inc" -I"/Users/Nico/Documents/SeCube_Project/Code/Optimized_APIs_Firmware_NonBlockingLogin_newDebug/SEcube_SDK/Development/Environment/Middlewares/ST/STM32_USB_Device_Library/Class/MSC/Inc" -I"/Users/Nico/Documents/SeCube_Project/Code/Optimized_APIs_Firmware_NonBlockingLogin_newDebug/SEcube_SDK/Development/Environment/Drivers/CMSIS/Include" -I"/Users/Nico/Documents/SeCube_Project/Code/Optimized_APIs_Firmware_NonBlockingLogin_newDebug/SEcube_SDK/Development/Environment/Drivers/CMSIS/Device/ST/STM32F4xx/Include" -I"/Users/Nico/Documents/SeCube_Project/Code/Optimized_APIs_Firmware_NonBlockingLogin_newDebug/SEcube_SDK/Development/Environment/secube/src" -I"/Users/Nico/Documents/SeCube_Project/Code/Optimized_APIs_Firmware_NonBlockingLogin_newDebug/SEcube_SDK/Development/Environment/secube/src/Common" -I"/Users/Nico/Documents/SeCube_Project/Code/Optimized_APIs_Firmware_NonBlockingLogin_newDebug/SEcube_SDK/Development/Environment/secube/src/Device"  -O0 -g3 -Wall -fmessage-length=0 -ffunction-sections -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

Application/src/fmc.o: /Users/Nico/Documents/SeCube_Project/Code/Optimized_APIs_Firmware_NonBlockingLogin_newDebug/SEcube_SDK/Development/Environment/secube/src/fmc.c
	@echo 'Building file: $<'
	@echo 'Invoking: MCU GCC Compiler'
	@echo $(PWD)
	arm-none-eabi-gcc -mcpu=cortex-m4 -mthumb -mfloat-abi=hard -mfpu=fpv4-sp-d16 -std=c99 '-D__weak=__attribute__((weak))' '-D__packed="__attribute__((__packed__))"' -DUSE_HAL_DRIVER -DSTM32F429xx -I"/Users/Nico/Documents/SeCube_Project/Code/Optimized_APIs_Firmware_NonBlockingLogin_newDebug/SEcube_SDK/Development/Environment/Drivers/STM32F4xx_HAL_Driver/Inc" -I"/Users/Nico/Documents/SeCube_Project/Code/Optimized_APIs_Firmware_NonBlockingLogin_newDebug/SEcube_SDK/Development/Environment/Drivers/STM32F4xx_HAL_Driver/Inc/Legacy" -I"/Users/Nico/Documents/SeCube_Project/Code/Optimized_APIs_Firmware_NonBlockingLogin_newDebug/SEcube_SDK/Development/Environment/Middlewares/ST/STM32_USB_Device_Library/Core/Inc" -I"/Users/Nico/Documents/SeCube_Project/Code/Optimized_APIs_Firmware_NonBlockingLogin_newDebug/SEcube_SDK/Development/Environment/Middlewares/ST/STM32_USB_Device_Library/Class/MSC/Inc" -I"/Users/Nico/Documents/SeCube_Project/Code/Optimized_APIs_Firmware_NonBlockingLogin_newDebug/SEcube_SDK/Development/Environment/Drivers/CMSIS/Include" -I"/Users/Nico/Documents/SeCube_Project/Code/Optimized_APIs_Firmware_NonBlockingLogin_newDebug/SEcube_SDK/Development/Environment/Drivers/CMSIS/Device/ST/STM32F4xx/Include" -I"/Users/Nico/Documents/SeCube_Project/Code/Optimized_APIs_Firmware_NonBlockingLogin_newDebug/SEcube_SDK/Development/Environment/secube/src" -I"/Users/Nico/Documents/SeCube_Project/Code/Optimized_APIs_Firmware_NonBlockingLogin_newDebug/SEcube_SDK/Development/Environment/secube/src/Common" -I"/Users/Nico/Documents/SeCube_Project/Code/Optimized_APIs_Firmware_NonBlockingLogin_newDebug/SEcube_SDK/Development/Environment/secube/src/Device"  -O0 -g3 -Wall -fmessage-length=0 -ffunction-sections -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

Application/src/gpio.o: /Users/Nico/Documents/SeCube_Project/Code/Optimized_APIs_Firmware_NonBlockingLogin_newDebug/SEcube_SDK/Development/Environment/secube/src/gpio.c
	@echo 'Building file: $<'
	@echo 'Invoking: MCU GCC Compiler'
	@echo $(PWD)
	arm-none-eabi-gcc -mcpu=cortex-m4 -mthumb -mfloat-abi=hard -mfpu=fpv4-sp-d16 -std=c99 '-D__weak=__attribute__((weak))' '-D__packed="__attribute__((__packed__))"' -DUSE_HAL_DRIVER -DSTM32F429xx -I"/Users/Nico/Documents/SeCube_Project/Code/Optimized_APIs_Firmware_NonBlockingLogin_newDebug/SEcube_SDK/Development/Environment/Drivers/STM32F4xx_HAL_Driver/Inc" -I"/Users/Nico/Documents/SeCube_Project/Code/Optimized_APIs_Firmware_NonBlockingLogin_newDebug/SEcube_SDK/Development/Environment/Drivers/STM32F4xx_HAL_Driver/Inc/Legacy" -I"/Users/Nico/Documents/SeCube_Project/Code/Optimized_APIs_Firmware_NonBlockingLogin_newDebug/SEcube_SDK/Development/Environment/Middlewares/ST/STM32_USB_Device_Library/Core/Inc" -I"/Users/Nico/Documents/SeCube_Project/Code/Optimized_APIs_Firmware_NonBlockingLogin_newDebug/SEcube_SDK/Development/Environment/Middlewares/ST/STM32_USB_Device_Library/Class/MSC/Inc" -I"/Users/Nico/Documents/SeCube_Project/Code/Optimized_APIs_Firmware_NonBlockingLogin_newDebug/SEcube_SDK/Development/Environment/Drivers/CMSIS/Include" -I"/Users/Nico/Documents/SeCube_Project/Code/Optimized_APIs_Firmware_NonBlockingLogin_newDebug/SEcube_SDK/Development/Environment/Drivers/CMSIS/Device/ST/STM32F4xx/Include" -I"/Users/Nico/Documents/SeCube_Project/Code/Optimized_APIs_Firmware_NonBlockingLogin_newDebug/SEcube_SDK/Development/Environment/secube/src" -I"/Users/Nico/Documents/SeCube_Project/Code/Optimized_APIs_Firmware_NonBlockingLogin_newDebug/SEcube_SDK/Development/Environment/secube/src/Common" -I"/Users/Nico/Documents/SeCube_Project/Code/Optimized_APIs_Firmware_NonBlockingLogin_newDebug/SEcube_SDK/Development/Environment/secube/src/Device"  -O0 -g3 -Wall -fmessage-length=0 -ffunction-sections -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

Application/src/i2c.o: /Users/Nico/Documents/SeCube_Project/Code/Optimized_APIs_Firmware_NonBlockingLogin_newDebug/SEcube_SDK/Development/Environment/secube/src/i2c.c
	@echo 'Building file: $<'
	@echo 'Invoking: MCU GCC Compiler'
	@echo $(PWD)
	arm-none-eabi-gcc -mcpu=cortex-m4 -mthumb -mfloat-abi=hard -mfpu=fpv4-sp-d16 -std=c99 '-D__weak=__attribute__((weak))' '-D__packed="__attribute__((__packed__))"' -DUSE_HAL_DRIVER -DSTM32F429xx -I"/Users/Nico/Documents/SeCube_Project/Code/Optimized_APIs_Firmware_NonBlockingLogin_newDebug/SEcube_SDK/Development/Environment/Drivers/STM32F4xx_HAL_Driver/Inc" -I"/Users/Nico/Documents/SeCube_Project/Code/Optimized_APIs_Firmware_NonBlockingLogin_newDebug/SEcube_SDK/Development/Environment/Drivers/STM32F4xx_HAL_Driver/Inc/Legacy" -I"/Users/Nico/Documents/SeCube_Project/Code/Optimized_APIs_Firmware_NonBlockingLogin_newDebug/SEcube_SDK/Development/Environment/Middlewares/ST/STM32_USB_Device_Library/Core/Inc" -I"/Users/Nico/Documents/SeCube_Project/Code/Optimized_APIs_Firmware_NonBlockingLogin_newDebug/SEcube_SDK/Development/Environment/Middlewares/ST/STM32_USB_Device_Library/Class/MSC/Inc" -I"/Users/Nico/Documents/SeCube_Project/Code/Optimized_APIs_Firmware_NonBlockingLogin_newDebug/SEcube_SDK/Development/Environment/Drivers/CMSIS/Include" -I"/Users/Nico/Documents/SeCube_Project/Code/Optimized_APIs_Firmware_NonBlockingLogin_newDebug/SEcube_SDK/Development/Environment/Drivers/CMSIS/Device/ST/STM32F4xx/Include" -I"/Users/Nico/Documents/SeCube_Project/Code/Optimized_APIs_Firmware_NonBlockingLogin_newDebug/SEcube_SDK/Development/Environment/secube/src" -I"/Users/Nico/Documents/SeCube_Project/Code/Optimized_APIs_Firmware_NonBlockingLogin_newDebug/SEcube_SDK/Development/Environment/secube/src/Common" -I"/Users/Nico/Documents/SeCube_Project/Code/Optimized_APIs_Firmware_NonBlockingLogin_newDebug/SEcube_SDK/Development/Environment/secube/src/Device"  -O0 -g3 -Wall -fmessage-length=0 -ffunction-sections -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

Application/src/main.o: /Users/Nico/Documents/SeCube_Project/Code/Optimized_APIs_Firmware_NonBlockingLogin_newDebug/SEcube_SDK/Development/Environment/secube/src/main.c
	@echo 'Building file: $<'
	@echo 'Invoking: MCU GCC Compiler'
	@echo $(PWD)
	arm-none-eabi-gcc -mcpu=cortex-m4 -mthumb -mfloat-abi=hard -mfpu=fpv4-sp-d16 -std=c99 '-D__weak=__attribute__((weak))' '-D__packed="__attribute__((__packed__))"' -DUSE_HAL_DRIVER -DSTM32F429xx -I"/Users/Nico/Documents/SeCube_Project/Code/Optimized_APIs_Firmware_NonBlockingLogin_newDebug/SEcube_SDK/Development/Environment/Drivers/STM32F4xx_HAL_Driver/Inc" -I"/Users/Nico/Documents/SeCube_Project/Code/Optimized_APIs_Firmware_NonBlockingLogin_newDebug/SEcube_SDK/Development/Environment/Drivers/STM32F4xx_HAL_Driver/Inc/Legacy" -I"/Users/Nico/Documents/SeCube_Project/Code/Optimized_APIs_Firmware_NonBlockingLogin_newDebug/SEcube_SDK/Development/Environment/Middlewares/ST/STM32_USB_Device_Library/Core/Inc" -I"/Users/Nico/Documents/SeCube_Project/Code/Optimized_APIs_Firmware_NonBlockingLogin_newDebug/SEcube_SDK/Development/Environment/Middlewares/ST/STM32_USB_Device_Library/Class/MSC/Inc" -I"/Users/Nico/Documents/SeCube_Project/Code/Optimized_APIs_Firmware_NonBlockingLogin_newDebug/SEcube_SDK/Development/Environment/Drivers/CMSIS/Include" -I"/Users/Nico/Documents/SeCube_Project/Code/Optimized_APIs_Firmware_NonBlockingLogin_newDebug/SEcube_SDK/Development/Environment/Drivers/CMSIS/Device/ST/STM32F4xx/Include" -I"/Users/Nico/Documents/SeCube_Project/Code/Optimized_APIs_Firmware_NonBlockingLogin_newDebug/SEcube_SDK/Development/Environment/secube/src" -I"/Users/Nico/Documents/SeCube_Project/Code/Optimized_APIs_Firmware_NonBlockingLogin_newDebug/SEcube_SDK/Development/Environment/secube/src/Common" -I"/Users/Nico/Documents/SeCube_Project/Code/Optimized_APIs_Firmware_NonBlockingLogin_newDebug/SEcube_SDK/Development/Environment/secube/src/Device"  -O0 -g3 -Wall -fmessage-length=0 -ffunction-sections -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

Application/src/rng.o: /Users/Nico/Documents/SeCube_Project/Code/Optimized_APIs_Firmware_NonBlockingLogin_newDebug/SEcube_SDK/Development/Environment/secube/src/rng.c
	@echo 'Building file: $<'
	@echo 'Invoking: MCU GCC Compiler'
	@echo $(PWD)
	arm-none-eabi-gcc -mcpu=cortex-m4 -mthumb -mfloat-abi=hard -mfpu=fpv4-sp-d16 -std=c99 '-D__weak=__attribute__((weak))' '-D__packed="__attribute__((__packed__))"' -DUSE_HAL_DRIVER -DSTM32F429xx -I"/Users/Nico/Documents/SeCube_Project/Code/Optimized_APIs_Firmware_NonBlockingLogin_newDebug/SEcube_SDK/Development/Environment/Drivers/STM32F4xx_HAL_Driver/Inc" -I"/Users/Nico/Documents/SeCube_Project/Code/Optimized_APIs_Firmware_NonBlockingLogin_newDebug/SEcube_SDK/Development/Environment/Drivers/STM32F4xx_HAL_Driver/Inc/Legacy" -I"/Users/Nico/Documents/SeCube_Project/Code/Optimized_APIs_Firmware_NonBlockingLogin_newDebug/SEcube_SDK/Development/Environment/Middlewares/ST/STM32_USB_Device_Library/Core/Inc" -I"/Users/Nico/Documents/SeCube_Project/Code/Optimized_APIs_Firmware_NonBlockingLogin_newDebug/SEcube_SDK/Development/Environment/Middlewares/ST/STM32_USB_Device_Library/Class/MSC/Inc" -I"/Users/Nico/Documents/SeCube_Project/Code/Optimized_APIs_Firmware_NonBlockingLogin_newDebug/SEcube_SDK/Development/Environment/Drivers/CMSIS/Include" -I"/Users/Nico/Documents/SeCube_Project/Code/Optimized_APIs_Firmware_NonBlockingLogin_newDebug/SEcube_SDK/Development/Environment/Drivers/CMSIS/Device/ST/STM32F4xx/Include" -I"/Users/Nico/Documents/SeCube_Project/Code/Optimized_APIs_Firmware_NonBlockingLogin_newDebug/SEcube_SDK/Development/Environment/secube/src" -I"/Users/Nico/Documents/SeCube_Project/Code/Optimized_APIs_Firmware_NonBlockingLogin_newDebug/SEcube_SDK/Development/Environment/secube/src/Common" -I"/Users/Nico/Documents/SeCube_Project/Code/Optimized_APIs_Firmware_NonBlockingLogin_newDebug/SEcube_SDK/Development/Environment/secube/src/Device"  -O0 -g3 -Wall -fmessage-length=0 -ffunction-sections -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

Application/src/sdio.o: /Users/Nico/Documents/SeCube_Project/Code/Optimized_APIs_Firmware_NonBlockingLogin_newDebug/SEcube_SDK/Development/Environment/secube/src/sdio.c
	@echo 'Building file: $<'
	@echo 'Invoking: MCU GCC Compiler'
	@echo $(PWD)
	arm-none-eabi-gcc -mcpu=cortex-m4 -mthumb -mfloat-abi=hard -mfpu=fpv4-sp-d16 -std=c99 '-D__weak=__attribute__((weak))' '-D__packed="__attribute__((__packed__))"' -DUSE_HAL_DRIVER -DSTM32F429xx -I"/Users/Nico/Documents/SeCube_Project/Code/Optimized_APIs_Firmware_NonBlockingLogin_newDebug/SEcube_SDK/Development/Environment/Drivers/STM32F4xx_HAL_Driver/Inc" -I"/Users/Nico/Documents/SeCube_Project/Code/Optimized_APIs_Firmware_NonBlockingLogin_newDebug/SEcube_SDK/Development/Environment/Drivers/STM32F4xx_HAL_Driver/Inc/Legacy" -I"/Users/Nico/Documents/SeCube_Project/Code/Optimized_APIs_Firmware_NonBlockingLogin_newDebug/SEcube_SDK/Development/Environment/Middlewares/ST/STM32_USB_Device_Library/Core/Inc" -I"/Users/Nico/Documents/SeCube_Project/Code/Optimized_APIs_Firmware_NonBlockingLogin_newDebug/SEcube_SDK/Development/Environment/Middlewares/ST/STM32_USB_Device_Library/Class/MSC/Inc" -I"/Users/Nico/Documents/SeCube_Project/Code/Optimized_APIs_Firmware_NonBlockingLogin_newDebug/SEcube_SDK/Development/Environment/Drivers/CMSIS/Include" -I"/Users/Nico/Documents/SeCube_Project/Code/Optimized_APIs_Firmware_NonBlockingLogin_newDebug/SEcube_SDK/Development/Environment/Drivers/CMSIS/Device/ST/STM32F4xx/Include" -I"/Users/Nico/Documents/SeCube_Project/Code/Optimized_APIs_Firmware_NonBlockingLogin_newDebug/SEcube_SDK/Development/Environment/secube/src" -I"/Users/Nico/Documents/SeCube_Project/Code/Optimized_APIs_Firmware_NonBlockingLogin_newDebug/SEcube_SDK/Development/Environment/secube/src/Common" -I"/Users/Nico/Documents/SeCube_Project/Code/Optimized_APIs_Firmware_NonBlockingLogin_newDebug/SEcube_SDK/Development/Environment/secube/src/Device"  -O0 -g3 -Wall -fmessage-length=0 -ffunction-sections -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

Application/src/se3_rand.o: /Users/Nico/Documents/SeCube_Project/Code/Optimized_APIs_Firmware_NonBlockingLogin_newDebug/SEcube_SDK/Development/Environment/secube/src/se3_rand.c
	@echo 'Building file: $<'
	@echo 'Invoking: MCU GCC Compiler'
	@echo $(PWD)
	arm-none-eabi-gcc -mcpu=cortex-m4 -mthumb -mfloat-abi=hard -mfpu=fpv4-sp-d16 -std=c99 '-D__weak=__attribute__((weak))' '-D__packed="__attribute__((__packed__))"' -DUSE_HAL_DRIVER -DSTM32F429xx -I"/Users/Nico/Documents/SeCube_Project/Code/Optimized_APIs_Firmware_NonBlockingLogin_newDebug/SEcube_SDK/Development/Environment/Drivers/STM32F4xx_HAL_Driver/Inc" -I"/Users/Nico/Documents/SeCube_Project/Code/Optimized_APIs_Firmware_NonBlockingLogin_newDebug/SEcube_SDK/Development/Environment/Drivers/STM32F4xx_HAL_Driver/Inc/Legacy" -I"/Users/Nico/Documents/SeCube_Project/Code/Optimized_APIs_Firmware_NonBlockingLogin_newDebug/SEcube_SDK/Development/Environment/Middlewares/ST/STM32_USB_Device_Library/Core/Inc" -I"/Users/Nico/Documents/SeCube_Project/Code/Optimized_APIs_Firmware_NonBlockingLogin_newDebug/SEcube_SDK/Development/Environment/Middlewares/ST/STM32_USB_Device_Library/Class/MSC/Inc" -I"/Users/Nico/Documents/SeCube_Project/Code/Optimized_APIs_Firmware_NonBlockingLogin_newDebug/SEcube_SDK/Development/Environment/Drivers/CMSIS/Include" -I"/Users/Nico/Documents/SeCube_Project/Code/Optimized_APIs_Firmware_NonBlockingLogin_newDebug/SEcube_SDK/Development/Environment/Drivers/CMSIS/Device/ST/STM32F4xx/Include" -I"/Users/Nico/Documents/SeCube_Project/Code/Optimized_APIs_Firmware_NonBlockingLogin_newDebug/SEcube_SDK/Development/Environment/secube/src" -I"/Users/Nico/Documents/SeCube_Project/Code/Optimized_APIs_Firmware_NonBlockingLogin_newDebug/SEcube_SDK/Development/Environment/secube/src/Common" -I"/Users/Nico/Documents/SeCube_Project/Code/Optimized_APIs_Firmware_NonBlockingLogin_newDebug/SEcube_SDK/Development/Environment/secube/src/Device"  -O0 -g3 -Wall -fmessage-length=0 -ffunction-sections -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

Application/src/se3_sdio.o: /Users/Nico/Documents/SeCube_Project/Code/Optimized_APIs_Firmware_NonBlockingLogin_newDebug/SEcube_SDK/Development/Environment/secube/src/se3_sdio.c
	@echo 'Building file: $<'
	@echo 'Invoking: MCU GCC Compiler'
	@echo $(PWD)
	arm-none-eabi-gcc -mcpu=cortex-m4 -mthumb -mfloat-abi=hard -mfpu=fpv4-sp-d16 -std=c99 '-D__weak=__attribute__((weak))' '-D__packed="__attribute__((__packed__))"' -DUSE_HAL_DRIVER -DSTM32F429xx -I"/Users/Nico/Documents/SeCube_Project/Code/Optimized_APIs_Firmware_NonBlockingLogin_newDebug/SEcube_SDK/Development/Environment/Drivers/STM32F4xx_HAL_Driver/Inc" -I"/Users/Nico/Documents/SeCube_Project/Code/Optimized_APIs_Firmware_NonBlockingLogin_newDebug/SEcube_SDK/Development/Environment/Drivers/STM32F4xx_HAL_Driver/Inc/Legacy" -I"/Users/Nico/Documents/SeCube_Project/Code/Optimized_APIs_Firmware_NonBlockingLogin_newDebug/SEcube_SDK/Development/Environment/Middlewares/ST/STM32_USB_Device_Library/Core/Inc" -I"/Users/Nico/Documents/SeCube_Project/Code/Optimized_APIs_Firmware_NonBlockingLogin_newDebug/SEcube_SDK/Development/Environment/Middlewares/ST/STM32_USB_Device_Library/Class/MSC/Inc" -I"/Users/Nico/Documents/SeCube_Project/Code/Optimized_APIs_Firmware_NonBlockingLogin_newDebug/SEcube_SDK/Development/Environment/Drivers/CMSIS/Include" -I"/Users/Nico/Documents/SeCube_Project/Code/Optimized_APIs_Firmware_NonBlockingLogin_newDebug/SEcube_SDK/Development/Environment/Drivers/CMSIS/Device/ST/STM32F4xx/Include" -I"/Users/Nico/Documents/SeCube_Project/Code/Optimized_APIs_Firmware_NonBlockingLogin_newDebug/SEcube_SDK/Development/Environment/secube/src" -I"/Users/Nico/Documents/SeCube_Project/Code/Optimized_APIs_Firmware_NonBlockingLogin_newDebug/SEcube_SDK/Development/Environment/secube/src/Common" -I"/Users/Nico/Documents/SeCube_Project/Code/Optimized_APIs_Firmware_NonBlockingLogin_newDebug/SEcube_SDK/Development/Environment/secube/src/Device"  -O0 -g3 -Wall -fmessage-length=0 -ffunction-sections -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

Application/src/spi.o: /Users/Nico/Documents/SeCube_Project/Code/Optimized_APIs_Firmware_NonBlockingLogin_newDebug/SEcube_SDK/Development/Environment/secube/src/spi.c
	@echo 'Building file: $<'
	@echo 'Invoking: MCU GCC Compiler'
	@echo $(PWD)
	arm-none-eabi-gcc -mcpu=cortex-m4 -mthumb -mfloat-abi=hard -mfpu=fpv4-sp-d16 -std=c99 '-D__weak=__attribute__((weak))' '-D__packed="__attribute__((__packed__))"' -DUSE_HAL_DRIVER -DSTM32F429xx -I"/Users/Nico/Documents/SeCube_Project/Code/Optimized_APIs_Firmware_NonBlockingLogin_newDebug/SEcube_SDK/Development/Environment/Drivers/STM32F4xx_HAL_Driver/Inc" -I"/Users/Nico/Documents/SeCube_Project/Code/Optimized_APIs_Firmware_NonBlockingLogin_newDebug/SEcube_SDK/Development/Environment/Drivers/STM32F4xx_HAL_Driver/Inc/Legacy" -I"/Users/Nico/Documents/SeCube_Project/Code/Optimized_APIs_Firmware_NonBlockingLogin_newDebug/SEcube_SDK/Development/Environment/Middlewares/ST/STM32_USB_Device_Library/Core/Inc" -I"/Users/Nico/Documents/SeCube_Project/Code/Optimized_APIs_Firmware_NonBlockingLogin_newDebug/SEcube_SDK/Development/Environment/Middlewares/ST/STM32_USB_Device_Library/Class/MSC/Inc" -I"/Users/Nico/Documents/SeCube_Project/Code/Optimized_APIs_Firmware_NonBlockingLogin_newDebug/SEcube_SDK/Development/Environment/Drivers/CMSIS/Include" -I"/Users/Nico/Documents/SeCube_Project/Code/Optimized_APIs_Firmware_NonBlockingLogin_newDebug/SEcube_SDK/Development/Environment/Drivers/CMSIS/Device/ST/STM32F4xx/Include" -I"/Users/Nico/Documents/SeCube_Project/Code/Optimized_APIs_Firmware_NonBlockingLogin_newDebug/SEcube_SDK/Development/Environment/secube/src" -I"/Users/Nico/Documents/SeCube_Project/Code/Optimized_APIs_Firmware_NonBlockingLogin_newDebug/SEcube_SDK/Development/Environment/secube/src/Common" -I"/Users/Nico/Documents/SeCube_Project/Code/Optimized_APIs_Firmware_NonBlockingLogin_newDebug/SEcube_SDK/Development/Environment/secube/src/Device"  -O0 -g3 -Wall -fmessage-length=0 -ffunction-sections -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

Application/src/stm32f4xx_hal_msp.o: /Users/Nico/Documents/SeCube_Project/Code/Optimized_APIs_Firmware_NonBlockingLogin_newDebug/SEcube_SDK/Development/Environment/secube/src/stm32f4xx_hal_msp.c
	@echo 'Building file: $<'
	@echo 'Invoking: MCU GCC Compiler'
	@echo $(PWD)
	arm-none-eabi-gcc -mcpu=cortex-m4 -mthumb -mfloat-abi=hard -mfpu=fpv4-sp-d16 -std=c99 '-D__weak=__attribute__((weak))' '-D__packed="__attribute__((__packed__))"' -DUSE_HAL_DRIVER -DSTM32F429xx -I"/Users/Nico/Documents/SeCube_Project/Code/Optimized_APIs_Firmware_NonBlockingLogin_newDebug/SEcube_SDK/Development/Environment/Drivers/STM32F4xx_HAL_Driver/Inc" -I"/Users/Nico/Documents/SeCube_Project/Code/Optimized_APIs_Firmware_NonBlockingLogin_newDebug/SEcube_SDK/Development/Environment/Drivers/STM32F4xx_HAL_Driver/Inc/Legacy" -I"/Users/Nico/Documents/SeCube_Project/Code/Optimized_APIs_Firmware_NonBlockingLogin_newDebug/SEcube_SDK/Development/Environment/Middlewares/ST/STM32_USB_Device_Library/Core/Inc" -I"/Users/Nico/Documents/SeCube_Project/Code/Optimized_APIs_Firmware_NonBlockingLogin_newDebug/SEcube_SDK/Development/Environment/Middlewares/ST/STM32_USB_Device_Library/Class/MSC/Inc" -I"/Users/Nico/Documents/SeCube_Project/Code/Optimized_APIs_Firmware_NonBlockingLogin_newDebug/SEcube_SDK/Development/Environment/Drivers/CMSIS/Include" -I"/Users/Nico/Documents/SeCube_Project/Code/Optimized_APIs_Firmware_NonBlockingLogin_newDebug/SEcube_SDK/Development/Environment/Drivers/CMSIS/Device/ST/STM32F4xx/Include" -I"/Users/Nico/Documents/SeCube_Project/Code/Optimized_APIs_Firmware_NonBlockingLogin_newDebug/SEcube_SDK/Development/Environment/secube/src" -I"/Users/Nico/Documents/SeCube_Project/Code/Optimized_APIs_Firmware_NonBlockingLogin_newDebug/SEcube_SDK/Development/Environment/secube/src/Common" -I"/Users/Nico/Documents/SeCube_Project/Code/Optimized_APIs_Firmware_NonBlockingLogin_newDebug/SEcube_SDK/Development/Environment/secube/src/Device"  -O0 -g3 -Wall -fmessage-length=0 -ffunction-sections -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

Application/src/stm32f4xx_it.o: /Users/Nico/Documents/SeCube_Project/Code/Optimized_APIs_Firmware_NonBlockingLogin_newDebug/SEcube_SDK/Development/Environment/secube/src/stm32f4xx_it.c
	@echo 'Building file: $<'
	@echo 'Invoking: MCU GCC Compiler'
	@echo $(PWD)
	arm-none-eabi-gcc -mcpu=cortex-m4 -mthumb -mfloat-abi=hard -mfpu=fpv4-sp-d16 -std=c99 '-D__weak=__attribute__((weak))' '-D__packed="__attribute__((__packed__))"' -DUSE_HAL_DRIVER -DSTM32F429xx -I"/Users/Nico/Documents/SeCube_Project/Code/Optimized_APIs_Firmware_NonBlockingLogin_newDebug/SEcube_SDK/Development/Environment/Drivers/STM32F4xx_HAL_Driver/Inc" -I"/Users/Nico/Documents/SeCube_Project/Code/Optimized_APIs_Firmware_NonBlockingLogin_newDebug/SEcube_SDK/Development/Environment/Drivers/STM32F4xx_HAL_Driver/Inc/Legacy" -I"/Users/Nico/Documents/SeCube_Project/Code/Optimized_APIs_Firmware_NonBlockingLogin_newDebug/SEcube_SDK/Development/Environment/Middlewares/ST/STM32_USB_Device_Library/Core/Inc" -I"/Users/Nico/Documents/SeCube_Project/Code/Optimized_APIs_Firmware_NonBlockingLogin_newDebug/SEcube_SDK/Development/Environment/Middlewares/ST/STM32_USB_Device_Library/Class/MSC/Inc" -I"/Users/Nico/Documents/SeCube_Project/Code/Optimized_APIs_Firmware_NonBlockingLogin_newDebug/SEcube_SDK/Development/Environment/Drivers/CMSIS/Include" -I"/Users/Nico/Documents/SeCube_Project/Code/Optimized_APIs_Firmware_NonBlockingLogin_newDebug/SEcube_SDK/Development/Environment/Drivers/CMSIS/Device/ST/STM32F4xx/Include" -I"/Users/Nico/Documents/SeCube_Project/Code/Optimized_APIs_Firmware_NonBlockingLogin_newDebug/SEcube_SDK/Development/Environment/secube/src" -I"/Users/Nico/Documents/SeCube_Project/Code/Optimized_APIs_Firmware_NonBlockingLogin_newDebug/SEcube_SDK/Development/Environment/secube/src/Common" -I"/Users/Nico/Documents/SeCube_Project/Code/Optimized_APIs_Firmware_NonBlockingLogin_newDebug/SEcube_SDK/Development/Environment/secube/src/Device"  -O0 -g3 -Wall -fmessage-length=0 -ffunction-sections -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

Application/src/tim.o: /Users/Nico/Documents/SeCube_Project/Code/Optimized_APIs_Firmware_NonBlockingLogin_newDebug/SEcube_SDK/Development/Environment/secube/src/tim.c
	@echo 'Building file: $<'
	@echo 'Invoking: MCU GCC Compiler'
	@echo $(PWD)
	arm-none-eabi-gcc -mcpu=cortex-m4 -mthumb -mfloat-abi=hard -mfpu=fpv4-sp-d16 -std=c99 '-D__weak=__attribute__((weak))' '-D__packed="__attribute__((__packed__))"' -DUSE_HAL_DRIVER -DSTM32F429xx -I"/Users/Nico/Documents/SeCube_Project/Code/Optimized_APIs_Firmware_NonBlockingLogin_newDebug/SEcube_SDK/Development/Environment/Drivers/STM32F4xx_HAL_Driver/Inc" -I"/Users/Nico/Documents/SeCube_Project/Code/Optimized_APIs_Firmware_NonBlockingLogin_newDebug/SEcube_SDK/Development/Environment/Drivers/STM32F4xx_HAL_Driver/Inc/Legacy" -I"/Users/Nico/Documents/SeCube_Project/Code/Optimized_APIs_Firmware_NonBlockingLogin_newDebug/SEcube_SDK/Development/Environment/Middlewares/ST/STM32_USB_Device_Library/Core/Inc" -I"/Users/Nico/Documents/SeCube_Project/Code/Optimized_APIs_Firmware_NonBlockingLogin_newDebug/SEcube_SDK/Development/Environment/Middlewares/ST/STM32_USB_Device_Library/Class/MSC/Inc" -I"/Users/Nico/Documents/SeCube_Project/Code/Optimized_APIs_Firmware_NonBlockingLogin_newDebug/SEcube_SDK/Development/Environment/Drivers/CMSIS/Include" -I"/Users/Nico/Documents/SeCube_Project/Code/Optimized_APIs_Firmware_NonBlockingLogin_newDebug/SEcube_SDK/Development/Environment/Drivers/CMSIS/Device/ST/STM32F4xx/Include" -I"/Users/Nico/Documents/SeCube_Project/Code/Optimized_APIs_Firmware_NonBlockingLogin_newDebug/SEcube_SDK/Development/Environment/secube/src" -I"/Users/Nico/Documents/SeCube_Project/Code/Optimized_APIs_Firmware_NonBlockingLogin_newDebug/SEcube_SDK/Development/Environment/secube/src/Common" -I"/Users/Nico/Documents/SeCube_Project/Code/Optimized_APIs_Firmware_NonBlockingLogin_newDebug/SEcube_SDK/Development/Environment/secube/src/Device"  -O0 -g3 -Wall -fmessage-length=0 -ffunction-sections -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

Application/src/usart.o: /Users/Nico/Documents/SeCube_Project/Code/Optimized_APIs_Firmware_NonBlockingLogin_newDebug/SEcube_SDK/Development/Environment/secube/src/usart.c
	@echo 'Building file: $<'
	@echo 'Invoking: MCU GCC Compiler'
	@echo $(PWD)
	arm-none-eabi-gcc -mcpu=cortex-m4 -mthumb -mfloat-abi=hard -mfpu=fpv4-sp-d16 -std=c99 '-D__weak=__attribute__((weak))' '-D__packed="__attribute__((__packed__))"' -DUSE_HAL_DRIVER -DSTM32F429xx -I"/Users/Nico/Documents/SeCube_Project/Code/Optimized_APIs_Firmware_NonBlockingLogin_newDebug/SEcube_SDK/Development/Environment/Drivers/STM32F4xx_HAL_Driver/Inc" -I"/Users/Nico/Documents/SeCube_Project/Code/Optimized_APIs_Firmware_NonBlockingLogin_newDebug/SEcube_SDK/Development/Environment/Drivers/STM32F4xx_HAL_Driver/Inc/Legacy" -I"/Users/Nico/Documents/SeCube_Project/Code/Optimized_APIs_Firmware_NonBlockingLogin_newDebug/SEcube_SDK/Development/Environment/Middlewares/ST/STM32_USB_Device_Library/Core/Inc" -I"/Users/Nico/Documents/SeCube_Project/Code/Optimized_APIs_Firmware_NonBlockingLogin_newDebug/SEcube_SDK/Development/Environment/Middlewares/ST/STM32_USB_Device_Library/Class/MSC/Inc" -I"/Users/Nico/Documents/SeCube_Project/Code/Optimized_APIs_Firmware_NonBlockingLogin_newDebug/SEcube_SDK/Development/Environment/Drivers/CMSIS/Include" -I"/Users/Nico/Documents/SeCube_Project/Code/Optimized_APIs_Firmware_NonBlockingLogin_newDebug/SEcube_SDK/Development/Environment/Drivers/CMSIS/Device/ST/STM32F4xx/Include" -I"/Users/Nico/Documents/SeCube_Project/Code/Optimized_APIs_Firmware_NonBlockingLogin_newDebug/SEcube_SDK/Development/Environment/secube/src" -I"/Users/Nico/Documents/SeCube_Project/Code/Optimized_APIs_Firmware_NonBlockingLogin_newDebug/SEcube_SDK/Development/Environment/secube/src/Common" -I"/Users/Nico/Documents/SeCube_Project/Code/Optimized_APIs_Firmware_NonBlockingLogin_newDebug/SEcube_SDK/Development/Environment/secube/src/Device"  -O0 -g3 -Wall -fmessage-length=0 -ffunction-sections -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

Application/src/usb_device.o: /Users/Nico/Documents/SeCube_Project/Code/Optimized_APIs_Firmware_NonBlockingLogin_newDebug/SEcube_SDK/Development/Environment/secube/src/usb_device.c
	@echo 'Building file: $<'
	@echo 'Invoking: MCU GCC Compiler'
	@echo $(PWD)
	arm-none-eabi-gcc -mcpu=cortex-m4 -mthumb -mfloat-abi=hard -mfpu=fpv4-sp-d16 -std=c99 '-D__weak=__attribute__((weak))' '-D__packed="__attribute__((__packed__))"' -DUSE_HAL_DRIVER -DSTM32F429xx -I"/Users/Nico/Documents/SeCube_Project/Code/Optimized_APIs_Firmware_NonBlockingLogin_newDebug/SEcube_SDK/Development/Environment/Drivers/STM32F4xx_HAL_Driver/Inc" -I"/Users/Nico/Documents/SeCube_Project/Code/Optimized_APIs_Firmware_NonBlockingLogin_newDebug/SEcube_SDK/Development/Environment/Drivers/STM32F4xx_HAL_Driver/Inc/Legacy" -I"/Users/Nico/Documents/SeCube_Project/Code/Optimized_APIs_Firmware_NonBlockingLogin_newDebug/SEcube_SDK/Development/Environment/Middlewares/ST/STM32_USB_Device_Library/Core/Inc" -I"/Users/Nico/Documents/SeCube_Project/Code/Optimized_APIs_Firmware_NonBlockingLogin_newDebug/SEcube_SDK/Development/Environment/Middlewares/ST/STM32_USB_Device_Library/Class/MSC/Inc" -I"/Users/Nico/Documents/SeCube_Project/Code/Optimized_APIs_Firmware_NonBlockingLogin_newDebug/SEcube_SDK/Development/Environment/Drivers/CMSIS/Include" -I"/Users/Nico/Documents/SeCube_Project/Code/Optimized_APIs_Firmware_NonBlockingLogin_newDebug/SEcube_SDK/Development/Environment/Drivers/CMSIS/Device/ST/STM32F4xx/Include" -I"/Users/Nico/Documents/SeCube_Project/Code/Optimized_APIs_Firmware_NonBlockingLogin_newDebug/SEcube_SDK/Development/Environment/secube/src" -I"/Users/Nico/Documents/SeCube_Project/Code/Optimized_APIs_Firmware_NonBlockingLogin_newDebug/SEcube_SDK/Development/Environment/secube/src/Common" -I"/Users/Nico/Documents/SeCube_Project/Code/Optimized_APIs_Firmware_NonBlockingLogin_newDebug/SEcube_SDK/Development/Environment/secube/src/Device"  -O0 -g3 -Wall -fmessage-length=0 -ffunction-sections -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

Application/src/usbd_conf.o: /Users/Nico/Documents/SeCube_Project/Code/Optimized_APIs_Firmware_NonBlockingLogin_newDebug/SEcube_SDK/Development/Environment/secube/src/usbd_conf.c
	@echo 'Building file: $<'
	@echo 'Invoking: MCU GCC Compiler'
	@echo $(PWD)
	arm-none-eabi-gcc -mcpu=cortex-m4 -mthumb -mfloat-abi=hard -mfpu=fpv4-sp-d16 -std=c99 '-D__weak=__attribute__((weak))' '-D__packed="__attribute__((__packed__))"' -DUSE_HAL_DRIVER -DSTM32F429xx -I"/Users/Nico/Documents/SeCube_Project/Code/Optimized_APIs_Firmware_NonBlockingLogin_newDebug/SEcube_SDK/Development/Environment/Drivers/STM32F4xx_HAL_Driver/Inc" -I"/Users/Nico/Documents/SeCube_Project/Code/Optimized_APIs_Firmware_NonBlockingLogin_newDebug/SEcube_SDK/Development/Environment/Drivers/STM32F4xx_HAL_Driver/Inc/Legacy" -I"/Users/Nico/Documents/SeCube_Project/Code/Optimized_APIs_Firmware_NonBlockingLogin_newDebug/SEcube_SDK/Development/Environment/Middlewares/ST/STM32_USB_Device_Library/Core/Inc" -I"/Users/Nico/Documents/SeCube_Project/Code/Optimized_APIs_Firmware_NonBlockingLogin_newDebug/SEcube_SDK/Development/Environment/Middlewares/ST/STM32_USB_Device_Library/Class/MSC/Inc" -I"/Users/Nico/Documents/SeCube_Project/Code/Optimized_APIs_Firmware_NonBlockingLogin_newDebug/SEcube_SDK/Development/Environment/Drivers/CMSIS/Include" -I"/Users/Nico/Documents/SeCube_Project/Code/Optimized_APIs_Firmware_NonBlockingLogin_newDebug/SEcube_SDK/Development/Environment/Drivers/CMSIS/Device/ST/STM32F4xx/Include" -I"/Users/Nico/Documents/SeCube_Project/Code/Optimized_APIs_Firmware_NonBlockingLogin_newDebug/SEcube_SDK/Development/Environment/secube/src" -I"/Users/Nico/Documents/SeCube_Project/Code/Optimized_APIs_Firmware_NonBlockingLogin_newDebug/SEcube_SDK/Development/Environment/secube/src/Common" -I"/Users/Nico/Documents/SeCube_Project/Code/Optimized_APIs_Firmware_NonBlockingLogin_newDebug/SEcube_SDK/Development/Environment/secube/src/Device"  -O0 -g3 -Wall -fmessage-length=0 -ffunction-sections -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

Application/src/usbd_desc.o: /Users/Nico/Documents/SeCube_Project/Code/Optimized_APIs_Firmware_NonBlockingLogin_newDebug/SEcube_SDK/Development/Environment/secube/src/usbd_desc.c
	@echo 'Building file: $<'
	@echo 'Invoking: MCU GCC Compiler'
	@echo $(PWD)
	arm-none-eabi-gcc -mcpu=cortex-m4 -mthumb -mfloat-abi=hard -mfpu=fpv4-sp-d16 -std=c99 '-D__weak=__attribute__((weak))' '-D__packed="__attribute__((__packed__))"' -DUSE_HAL_DRIVER -DSTM32F429xx -I"/Users/Nico/Documents/SeCube_Project/Code/Optimized_APIs_Firmware_NonBlockingLogin_newDebug/SEcube_SDK/Development/Environment/Drivers/STM32F4xx_HAL_Driver/Inc" -I"/Users/Nico/Documents/SeCube_Project/Code/Optimized_APIs_Firmware_NonBlockingLogin_newDebug/SEcube_SDK/Development/Environment/Drivers/STM32F4xx_HAL_Driver/Inc/Legacy" -I"/Users/Nico/Documents/SeCube_Project/Code/Optimized_APIs_Firmware_NonBlockingLogin_newDebug/SEcube_SDK/Development/Environment/Middlewares/ST/STM32_USB_Device_Library/Core/Inc" -I"/Users/Nico/Documents/SeCube_Project/Code/Optimized_APIs_Firmware_NonBlockingLogin_newDebug/SEcube_SDK/Development/Environment/Middlewares/ST/STM32_USB_Device_Library/Class/MSC/Inc" -I"/Users/Nico/Documents/SeCube_Project/Code/Optimized_APIs_Firmware_NonBlockingLogin_newDebug/SEcube_SDK/Development/Environment/Drivers/CMSIS/Include" -I"/Users/Nico/Documents/SeCube_Project/Code/Optimized_APIs_Firmware_NonBlockingLogin_newDebug/SEcube_SDK/Development/Environment/Drivers/CMSIS/Device/ST/STM32F4xx/Include" -I"/Users/Nico/Documents/SeCube_Project/Code/Optimized_APIs_Firmware_NonBlockingLogin_newDebug/SEcube_SDK/Development/Environment/secube/src" -I"/Users/Nico/Documents/SeCube_Project/Code/Optimized_APIs_Firmware_NonBlockingLogin_newDebug/SEcube_SDK/Development/Environment/secube/src/Common" -I"/Users/Nico/Documents/SeCube_Project/Code/Optimized_APIs_Firmware_NonBlockingLogin_newDebug/SEcube_SDK/Development/Environment/secube/src/Device"  -O0 -g3 -Wall -fmessage-length=0 -ffunction-sections -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

Application/src/usbd_storage_if.o: /Users/Nico/Documents/SeCube_Project/Code/Optimized_APIs_Firmware_NonBlockingLogin_newDebug/SEcube_SDK/Development/Environment/secube/src/usbd_storage_if.c
	@echo 'Building file: $<'
	@echo 'Invoking: MCU GCC Compiler'
	@echo $(PWD)
	arm-none-eabi-gcc -mcpu=cortex-m4 -mthumb -mfloat-abi=hard -mfpu=fpv4-sp-d16 -std=c99 '-D__weak=__attribute__((weak))' '-D__packed="__attribute__((__packed__))"' -DUSE_HAL_DRIVER -DSTM32F429xx -I"/Users/Nico/Documents/SeCube_Project/Code/Optimized_APIs_Firmware_NonBlockingLogin_newDebug/SEcube_SDK/Development/Environment/Drivers/STM32F4xx_HAL_Driver/Inc" -I"/Users/Nico/Documents/SeCube_Project/Code/Optimized_APIs_Firmware_NonBlockingLogin_newDebug/SEcube_SDK/Development/Environment/Drivers/STM32F4xx_HAL_Driver/Inc/Legacy" -I"/Users/Nico/Documents/SeCube_Project/Code/Optimized_APIs_Firmware_NonBlockingLogin_newDebug/SEcube_SDK/Development/Environment/Middlewares/ST/STM32_USB_Device_Library/Core/Inc" -I"/Users/Nico/Documents/SeCube_Project/Code/Optimized_APIs_Firmware_NonBlockingLogin_newDebug/SEcube_SDK/Development/Environment/Middlewares/ST/STM32_USB_Device_Library/Class/MSC/Inc" -I"/Users/Nico/Documents/SeCube_Project/Code/Optimized_APIs_Firmware_NonBlockingLogin_newDebug/SEcube_SDK/Development/Environment/Drivers/CMSIS/Include" -I"/Users/Nico/Documents/SeCube_Project/Code/Optimized_APIs_Firmware_NonBlockingLogin_newDebug/SEcube_SDK/Development/Environment/Drivers/CMSIS/Device/ST/STM32F4xx/Include" -I"/Users/Nico/Documents/SeCube_Project/Code/Optimized_APIs_Firmware_NonBlockingLogin_newDebug/SEcube_SDK/Development/Environment/secube/src" -I"/Users/Nico/Documents/SeCube_Project/Code/Optimized_APIs_Firmware_NonBlockingLogin_newDebug/SEcube_SDK/Development/Environment/secube/src/Common" -I"/Users/Nico/Documents/SeCube_Project/Code/Optimized_APIs_Firmware_NonBlockingLogin_newDebug/SEcube_SDK/Development/Environment/secube/src/Device"  -O0 -g3 -Wall -fmessage-length=0 -ffunction-sections -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '



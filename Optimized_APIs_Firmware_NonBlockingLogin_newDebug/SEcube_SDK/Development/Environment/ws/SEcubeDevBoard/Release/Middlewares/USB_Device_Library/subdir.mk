################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
/Users/Nico/Documents/SeCube_Project/Code/Optimized_APIs_Firmware_NonBlockingLogin_newDebug/SEcube_SDK/Development/Environment/Middlewares/ST/STM32_USB_Device_Library/Core/Src/usbd_core.c \
/Users/Nico/Documents/SeCube_Project/Code/Optimized_APIs_Firmware_NonBlockingLogin_newDebug/SEcube_SDK/Development/Environment/Middlewares/ST/STM32_USB_Device_Library/Core/Src/usbd_ctlreq.c \
/Users/Nico/Documents/SeCube_Project/Code/Optimized_APIs_Firmware_NonBlockingLogin_newDebug/SEcube_SDK/Development/Environment/Middlewares/ST/STM32_USB_Device_Library/Core/Src/usbd_ioreq.c \
/Users/Nico/Documents/SeCube_Project/Code/Optimized_APIs_Firmware_NonBlockingLogin_newDebug/SEcube_SDK/Development/Environment/Middlewares/ST/STM32_USB_Device_Library/Class/MSC/Src/usbd_msc.c \
/Users/Nico/Documents/SeCube_Project/Code/Optimized_APIs_Firmware_NonBlockingLogin_newDebug/SEcube_SDK/Development/Environment/Middlewares/ST/STM32_USB_Device_Library/Class/MSC/Src/usbd_msc_bot.c \
/Users/Nico/Documents/SeCube_Project/Code/Optimized_APIs_Firmware_NonBlockingLogin_newDebug/SEcube_SDK/Development/Environment/Middlewares/ST/STM32_USB_Device_Library/Class/MSC/Src/usbd_msc_data.c \
/Users/Nico/Documents/SeCube_Project/Code/Optimized_APIs_Firmware_NonBlockingLogin_newDebug/SEcube_SDK/Development/Environment/Middlewares/ST/STM32_USB_Device_Library/Class/MSC/Src/usbd_msc_scsi.c 

OBJS += \
./Middlewares/USB_Device_Library/usbd_core.o \
./Middlewares/USB_Device_Library/usbd_ctlreq.o \
./Middlewares/USB_Device_Library/usbd_ioreq.o \
./Middlewares/USB_Device_Library/usbd_msc.o \
./Middlewares/USB_Device_Library/usbd_msc_bot.o \
./Middlewares/USB_Device_Library/usbd_msc_data.o \
./Middlewares/USB_Device_Library/usbd_msc_scsi.o 

C_DEPS += \
./Middlewares/USB_Device_Library/usbd_core.d \
./Middlewares/USB_Device_Library/usbd_ctlreq.d \
./Middlewares/USB_Device_Library/usbd_ioreq.d \
./Middlewares/USB_Device_Library/usbd_msc.d \
./Middlewares/USB_Device_Library/usbd_msc_bot.d \
./Middlewares/USB_Device_Library/usbd_msc_data.d \
./Middlewares/USB_Device_Library/usbd_msc_scsi.d 


# Each subdirectory must supply rules for building sources it contributes
Middlewares/USB_Device_Library/usbd_core.o: /Users/Nico/Documents/SeCube_Project/Code/Optimized_APIs_Firmware_NonBlockingLogin_newDebug/SEcube_SDK/Development/Environment/Middlewares/ST/STM32_USB_Device_Library/Core/Src/usbd_core.c
	@echo 'Building file: $<'
	@echo 'Invoking: MCU GCC Compiler'
	@echo $(PWD)
	arm-none-eabi-gcc -mcpu=cortex-m4 -mthumb -mfloat-abi=hard -mfpu=fpv4-sp-d16 -std=c99 '-D__weak=__attribute__((weak))' '-D__packed="__attribute__((__packed__))"' -DUSE_HAL_DRIVER -DSTM32F429xx -I"/Users/Nico/Documents/SeCube_Project/Code/Optimized_APIs_Firmware_NonBlockingLogin_newDebug/SEcube_SDK/Development/Environment/Drivers/STM32F4xx_HAL_Driver/Inc" -I"/Users/Nico/Documents/SeCube_Project/Code/Optimized_APIs_Firmware_NonBlockingLogin_newDebug/SEcube_SDK/Development/Environment/Drivers/STM32F4xx_HAL_Driver/Inc/Legacy" -I"/Users/Nico/Documents/SeCube_Project/Code/Optimized_APIs_Firmware_NonBlockingLogin_newDebug/SEcube_SDK/Development/Environment/Middlewares/ST/STM32_USB_Device_Library/Core/Inc" -I"/Users/Nico/Documents/SeCube_Project/Code/Optimized_APIs_Firmware_NonBlockingLogin_newDebug/SEcube_SDK/Development/Environment/Middlewares/ST/STM32_USB_Device_Library/Class/MSC/Inc" -I"/Users/Nico/Documents/SeCube_Project/Code/Optimized_APIs_Firmware_NonBlockingLogin_newDebug/SEcube_SDK/Development/Environment/Drivers/CMSIS/Include" -I"/Users/Nico/Documents/SeCube_Project/Code/Optimized_APIs_Firmware_NonBlockingLogin_newDebug/SEcube_SDK/Development/Environment/Drivers/CMSIS/Device/ST/STM32F4xx/Include" -I"/Users/Nico/Documents/SeCube_Project/Code/Optimized_APIs_Firmware_NonBlockingLogin_newDebug/SEcube_SDK/Development/Environment/secube/src" -I"/Users/Nico/Documents/SeCube_Project/Code/Optimized_APIs_Firmware_NonBlockingLogin_newDebug/SEcube_SDK/Development/Environment/secube/src/Common" -I"/Users/Nico/Documents/SeCube_Project/Code/Optimized_APIs_Firmware_NonBlockingLogin_newDebug/SEcube_SDK/Development/Environment/secube/src/Device"  -O3 -Wall -fmessage-length=0 -ffunction-sections -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

Middlewares/USB_Device_Library/usbd_ctlreq.o: /Users/Nico/Documents/SeCube_Project/Code/Optimized_APIs_Firmware_NonBlockingLogin_newDebug/SEcube_SDK/Development/Environment/Middlewares/ST/STM32_USB_Device_Library/Core/Src/usbd_ctlreq.c
	@echo 'Building file: $<'
	@echo 'Invoking: MCU GCC Compiler'
	@echo $(PWD)
	arm-none-eabi-gcc -mcpu=cortex-m4 -mthumb -mfloat-abi=hard -mfpu=fpv4-sp-d16 -std=c99 '-D__weak=__attribute__((weak))' '-D__packed="__attribute__((__packed__))"' -DUSE_HAL_DRIVER -DSTM32F429xx -I"/Users/Nico/Documents/SeCube_Project/Code/Optimized_APIs_Firmware_NonBlockingLogin_newDebug/SEcube_SDK/Development/Environment/Drivers/STM32F4xx_HAL_Driver/Inc" -I"/Users/Nico/Documents/SeCube_Project/Code/Optimized_APIs_Firmware_NonBlockingLogin_newDebug/SEcube_SDK/Development/Environment/Drivers/STM32F4xx_HAL_Driver/Inc/Legacy" -I"/Users/Nico/Documents/SeCube_Project/Code/Optimized_APIs_Firmware_NonBlockingLogin_newDebug/SEcube_SDK/Development/Environment/Middlewares/ST/STM32_USB_Device_Library/Core/Inc" -I"/Users/Nico/Documents/SeCube_Project/Code/Optimized_APIs_Firmware_NonBlockingLogin_newDebug/SEcube_SDK/Development/Environment/Middlewares/ST/STM32_USB_Device_Library/Class/MSC/Inc" -I"/Users/Nico/Documents/SeCube_Project/Code/Optimized_APIs_Firmware_NonBlockingLogin_newDebug/SEcube_SDK/Development/Environment/Drivers/CMSIS/Include" -I"/Users/Nico/Documents/SeCube_Project/Code/Optimized_APIs_Firmware_NonBlockingLogin_newDebug/SEcube_SDK/Development/Environment/Drivers/CMSIS/Device/ST/STM32F4xx/Include" -I"/Users/Nico/Documents/SeCube_Project/Code/Optimized_APIs_Firmware_NonBlockingLogin_newDebug/SEcube_SDK/Development/Environment/secube/src" -I"/Users/Nico/Documents/SeCube_Project/Code/Optimized_APIs_Firmware_NonBlockingLogin_newDebug/SEcube_SDK/Development/Environment/secube/src/Common" -I"/Users/Nico/Documents/SeCube_Project/Code/Optimized_APIs_Firmware_NonBlockingLogin_newDebug/SEcube_SDK/Development/Environment/secube/src/Device"  -O3 -Wall -fmessage-length=0 -ffunction-sections -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

Middlewares/USB_Device_Library/usbd_ioreq.o: /Users/Nico/Documents/SeCube_Project/Code/Optimized_APIs_Firmware_NonBlockingLogin_newDebug/SEcube_SDK/Development/Environment/Middlewares/ST/STM32_USB_Device_Library/Core/Src/usbd_ioreq.c
	@echo 'Building file: $<'
	@echo 'Invoking: MCU GCC Compiler'
	@echo $(PWD)
	arm-none-eabi-gcc -mcpu=cortex-m4 -mthumb -mfloat-abi=hard -mfpu=fpv4-sp-d16 -std=c99 '-D__weak=__attribute__((weak))' '-D__packed="__attribute__((__packed__))"' -DUSE_HAL_DRIVER -DSTM32F429xx -I"/Users/Nico/Documents/SeCube_Project/Code/Optimized_APIs_Firmware_NonBlockingLogin_newDebug/SEcube_SDK/Development/Environment/Drivers/STM32F4xx_HAL_Driver/Inc" -I"/Users/Nico/Documents/SeCube_Project/Code/Optimized_APIs_Firmware_NonBlockingLogin_newDebug/SEcube_SDK/Development/Environment/Drivers/STM32F4xx_HAL_Driver/Inc/Legacy" -I"/Users/Nico/Documents/SeCube_Project/Code/Optimized_APIs_Firmware_NonBlockingLogin_newDebug/SEcube_SDK/Development/Environment/Middlewares/ST/STM32_USB_Device_Library/Core/Inc" -I"/Users/Nico/Documents/SeCube_Project/Code/Optimized_APIs_Firmware_NonBlockingLogin_newDebug/SEcube_SDK/Development/Environment/Middlewares/ST/STM32_USB_Device_Library/Class/MSC/Inc" -I"/Users/Nico/Documents/SeCube_Project/Code/Optimized_APIs_Firmware_NonBlockingLogin_newDebug/SEcube_SDK/Development/Environment/Drivers/CMSIS/Include" -I"/Users/Nico/Documents/SeCube_Project/Code/Optimized_APIs_Firmware_NonBlockingLogin_newDebug/SEcube_SDK/Development/Environment/Drivers/CMSIS/Device/ST/STM32F4xx/Include" -I"/Users/Nico/Documents/SeCube_Project/Code/Optimized_APIs_Firmware_NonBlockingLogin_newDebug/SEcube_SDK/Development/Environment/secube/src" -I"/Users/Nico/Documents/SeCube_Project/Code/Optimized_APIs_Firmware_NonBlockingLogin_newDebug/SEcube_SDK/Development/Environment/secube/src/Common" -I"/Users/Nico/Documents/SeCube_Project/Code/Optimized_APIs_Firmware_NonBlockingLogin_newDebug/SEcube_SDK/Development/Environment/secube/src/Device"  -O3 -Wall -fmessage-length=0 -ffunction-sections -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

Middlewares/USB_Device_Library/usbd_msc.o: /Users/Nico/Documents/SeCube_Project/Code/Optimized_APIs_Firmware_NonBlockingLogin_newDebug/SEcube_SDK/Development/Environment/Middlewares/ST/STM32_USB_Device_Library/Class/MSC/Src/usbd_msc.c
	@echo 'Building file: $<'
	@echo 'Invoking: MCU GCC Compiler'
	@echo $(PWD)
	arm-none-eabi-gcc -mcpu=cortex-m4 -mthumb -mfloat-abi=hard -mfpu=fpv4-sp-d16 -std=c99 '-D__weak=__attribute__((weak))' '-D__packed="__attribute__((__packed__))"' -DUSE_HAL_DRIVER -DSTM32F429xx -I"/Users/Nico/Documents/SeCube_Project/Code/Optimized_APIs_Firmware_NonBlockingLogin_newDebug/SEcube_SDK/Development/Environment/Drivers/STM32F4xx_HAL_Driver/Inc" -I"/Users/Nico/Documents/SeCube_Project/Code/Optimized_APIs_Firmware_NonBlockingLogin_newDebug/SEcube_SDK/Development/Environment/Drivers/STM32F4xx_HAL_Driver/Inc/Legacy" -I"/Users/Nico/Documents/SeCube_Project/Code/Optimized_APIs_Firmware_NonBlockingLogin_newDebug/SEcube_SDK/Development/Environment/Middlewares/ST/STM32_USB_Device_Library/Core/Inc" -I"/Users/Nico/Documents/SeCube_Project/Code/Optimized_APIs_Firmware_NonBlockingLogin_newDebug/SEcube_SDK/Development/Environment/Middlewares/ST/STM32_USB_Device_Library/Class/MSC/Inc" -I"/Users/Nico/Documents/SeCube_Project/Code/Optimized_APIs_Firmware_NonBlockingLogin_newDebug/SEcube_SDK/Development/Environment/Drivers/CMSIS/Include" -I"/Users/Nico/Documents/SeCube_Project/Code/Optimized_APIs_Firmware_NonBlockingLogin_newDebug/SEcube_SDK/Development/Environment/Drivers/CMSIS/Device/ST/STM32F4xx/Include" -I"/Users/Nico/Documents/SeCube_Project/Code/Optimized_APIs_Firmware_NonBlockingLogin_newDebug/SEcube_SDK/Development/Environment/secube/src" -I"/Users/Nico/Documents/SeCube_Project/Code/Optimized_APIs_Firmware_NonBlockingLogin_newDebug/SEcube_SDK/Development/Environment/secube/src/Common" -I"/Users/Nico/Documents/SeCube_Project/Code/Optimized_APIs_Firmware_NonBlockingLogin_newDebug/SEcube_SDK/Development/Environment/secube/src/Device"  -O3 -Wall -fmessage-length=0 -ffunction-sections -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

Middlewares/USB_Device_Library/usbd_msc_bot.o: /Users/Nico/Documents/SeCube_Project/Code/Optimized_APIs_Firmware_NonBlockingLogin_newDebug/SEcube_SDK/Development/Environment/Middlewares/ST/STM32_USB_Device_Library/Class/MSC/Src/usbd_msc_bot.c
	@echo 'Building file: $<'
	@echo 'Invoking: MCU GCC Compiler'
	@echo $(PWD)
	arm-none-eabi-gcc -mcpu=cortex-m4 -mthumb -mfloat-abi=hard -mfpu=fpv4-sp-d16 -std=c99 '-D__weak=__attribute__((weak))' '-D__packed="__attribute__((__packed__))"' -DUSE_HAL_DRIVER -DSTM32F429xx -I"/Users/Nico/Documents/SeCube_Project/Code/Optimized_APIs_Firmware_NonBlockingLogin_newDebug/SEcube_SDK/Development/Environment/Drivers/STM32F4xx_HAL_Driver/Inc" -I"/Users/Nico/Documents/SeCube_Project/Code/Optimized_APIs_Firmware_NonBlockingLogin_newDebug/SEcube_SDK/Development/Environment/Drivers/STM32F4xx_HAL_Driver/Inc/Legacy" -I"/Users/Nico/Documents/SeCube_Project/Code/Optimized_APIs_Firmware_NonBlockingLogin_newDebug/SEcube_SDK/Development/Environment/Middlewares/ST/STM32_USB_Device_Library/Core/Inc" -I"/Users/Nico/Documents/SeCube_Project/Code/Optimized_APIs_Firmware_NonBlockingLogin_newDebug/SEcube_SDK/Development/Environment/Middlewares/ST/STM32_USB_Device_Library/Class/MSC/Inc" -I"/Users/Nico/Documents/SeCube_Project/Code/Optimized_APIs_Firmware_NonBlockingLogin_newDebug/SEcube_SDK/Development/Environment/Drivers/CMSIS/Include" -I"/Users/Nico/Documents/SeCube_Project/Code/Optimized_APIs_Firmware_NonBlockingLogin_newDebug/SEcube_SDK/Development/Environment/Drivers/CMSIS/Device/ST/STM32F4xx/Include" -I"/Users/Nico/Documents/SeCube_Project/Code/Optimized_APIs_Firmware_NonBlockingLogin_newDebug/SEcube_SDK/Development/Environment/secube/src" -I"/Users/Nico/Documents/SeCube_Project/Code/Optimized_APIs_Firmware_NonBlockingLogin_newDebug/SEcube_SDK/Development/Environment/secube/src/Common" -I"/Users/Nico/Documents/SeCube_Project/Code/Optimized_APIs_Firmware_NonBlockingLogin_newDebug/SEcube_SDK/Development/Environment/secube/src/Device"  -O3 -Wall -fmessage-length=0 -ffunction-sections -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

Middlewares/USB_Device_Library/usbd_msc_data.o: /Users/Nico/Documents/SeCube_Project/Code/Optimized_APIs_Firmware_NonBlockingLogin_newDebug/SEcube_SDK/Development/Environment/Middlewares/ST/STM32_USB_Device_Library/Class/MSC/Src/usbd_msc_data.c
	@echo 'Building file: $<'
	@echo 'Invoking: MCU GCC Compiler'
	@echo $(PWD)
	arm-none-eabi-gcc -mcpu=cortex-m4 -mthumb -mfloat-abi=hard -mfpu=fpv4-sp-d16 -std=c99 '-D__weak=__attribute__((weak))' '-D__packed="__attribute__((__packed__))"' -DUSE_HAL_DRIVER -DSTM32F429xx -I"/Users/Nico/Documents/SeCube_Project/Code/Optimized_APIs_Firmware_NonBlockingLogin_newDebug/SEcube_SDK/Development/Environment/Drivers/STM32F4xx_HAL_Driver/Inc" -I"/Users/Nico/Documents/SeCube_Project/Code/Optimized_APIs_Firmware_NonBlockingLogin_newDebug/SEcube_SDK/Development/Environment/Drivers/STM32F4xx_HAL_Driver/Inc/Legacy" -I"/Users/Nico/Documents/SeCube_Project/Code/Optimized_APIs_Firmware_NonBlockingLogin_newDebug/SEcube_SDK/Development/Environment/Middlewares/ST/STM32_USB_Device_Library/Core/Inc" -I"/Users/Nico/Documents/SeCube_Project/Code/Optimized_APIs_Firmware_NonBlockingLogin_newDebug/SEcube_SDK/Development/Environment/Middlewares/ST/STM32_USB_Device_Library/Class/MSC/Inc" -I"/Users/Nico/Documents/SeCube_Project/Code/Optimized_APIs_Firmware_NonBlockingLogin_newDebug/SEcube_SDK/Development/Environment/Drivers/CMSIS/Include" -I"/Users/Nico/Documents/SeCube_Project/Code/Optimized_APIs_Firmware_NonBlockingLogin_newDebug/SEcube_SDK/Development/Environment/Drivers/CMSIS/Device/ST/STM32F4xx/Include" -I"/Users/Nico/Documents/SeCube_Project/Code/Optimized_APIs_Firmware_NonBlockingLogin_newDebug/SEcube_SDK/Development/Environment/secube/src" -I"/Users/Nico/Documents/SeCube_Project/Code/Optimized_APIs_Firmware_NonBlockingLogin_newDebug/SEcube_SDK/Development/Environment/secube/src/Common" -I"/Users/Nico/Documents/SeCube_Project/Code/Optimized_APIs_Firmware_NonBlockingLogin_newDebug/SEcube_SDK/Development/Environment/secube/src/Device"  -O3 -Wall -fmessage-length=0 -ffunction-sections -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

Middlewares/USB_Device_Library/usbd_msc_scsi.o: /Users/Nico/Documents/SeCube_Project/Code/Optimized_APIs_Firmware_NonBlockingLogin_newDebug/SEcube_SDK/Development/Environment/Middlewares/ST/STM32_USB_Device_Library/Class/MSC/Src/usbd_msc_scsi.c
	@echo 'Building file: $<'
	@echo 'Invoking: MCU GCC Compiler'
	@echo $(PWD)
	arm-none-eabi-gcc -mcpu=cortex-m4 -mthumb -mfloat-abi=hard -mfpu=fpv4-sp-d16 -std=c99 '-D__weak=__attribute__((weak))' '-D__packed="__attribute__((__packed__))"' -DUSE_HAL_DRIVER -DSTM32F429xx -I"/Users/Nico/Documents/SeCube_Project/Code/Optimized_APIs_Firmware_NonBlockingLogin_newDebug/SEcube_SDK/Development/Environment/Drivers/STM32F4xx_HAL_Driver/Inc" -I"/Users/Nico/Documents/SeCube_Project/Code/Optimized_APIs_Firmware_NonBlockingLogin_newDebug/SEcube_SDK/Development/Environment/Drivers/STM32F4xx_HAL_Driver/Inc/Legacy" -I"/Users/Nico/Documents/SeCube_Project/Code/Optimized_APIs_Firmware_NonBlockingLogin_newDebug/SEcube_SDK/Development/Environment/Middlewares/ST/STM32_USB_Device_Library/Core/Inc" -I"/Users/Nico/Documents/SeCube_Project/Code/Optimized_APIs_Firmware_NonBlockingLogin_newDebug/SEcube_SDK/Development/Environment/Middlewares/ST/STM32_USB_Device_Library/Class/MSC/Inc" -I"/Users/Nico/Documents/SeCube_Project/Code/Optimized_APIs_Firmware_NonBlockingLogin_newDebug/SEcube_SDK/Development/Environment/Drivers/CMSIS/Include" -I"/Users/Nico/Documents/SeCube_Project/Code/Optimized_APIs_Firmware_NonBlockingLogin_newDebug/SEcube_SDK/Development/Environment/Drivers/CMSIS/Device/ST/STM32F4xx/Include" -I"/Users/Nico/Documents/SeCube_Project/Code/Optimized_APIs_Firmware_NonBlockingLogin_newDebug/SEcube_SDK/Development/Environment/secube/src" -I"/Users/Nico/Documents/SeCube_Project/Code/Optimized_APIs_Firmware_NonBlockingLogin_newDebug/SEcube_SDK/Development/Environment/secube/src/Common" -I"/Users/Nico/Documents/SeCube_Project/Code/Optimized_APIs_Firmware_NonBlockingLogin_newDebug/SEcube_SDK/Development/Environment/secube/src/Device"  -O3 -Wall -fmessage-length=0 -ffunction-sections -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '



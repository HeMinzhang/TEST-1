################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
/Users/Nico/Documents/SeCube_Project/Code/Optimized_APIs_Firmware_NonBlockingLogin_newDebug/SEcube_SDK/Development/Environment/secube/src/Common/aes256.c \
/Users/Nico/Documents/SeCube_Project/Code/Optimized_APIs_Firmware_NonBlockingLogin_newDebug/SEcube_SDK/Development/Environment/secube/src/Common/crc16.c \
/Users/Nico/Documents/SeCube_Project/Code/Optimized_APIs_Firmware_NonBlockingLogin_newDebug/SEcube_SDK/Development/Environment/secube/src/Common/pbkdf2.c \
/Users/Nico/Documents/SeCube_Project/Code/Optimized_APIs_Firmware_NonBlockingLogin_newDebug/SEcube_SDK/Development/Environment/secube/src/Common/se3_common.c \
/Users/Nico/Documents/SeCube_Project/Code/Optimized_APIs_Firmware_NonBlockingLogin_newDebug/SEcube_SDK/Development/Environment/secube/src/Common/sha256.c 

OBJS += \
./Application/src/Common/aes256.o \
./Application/src/Common/crc16.o \
./Application/src/Common/pbkdf2.o \
./Application/src/Common/se3_common.o \
./Application/src/Common/sha256.o 

C_DEPS += \
./Application/src/Common/aes256.d \
./Application/src/Common/crc16.d \
./Application/src/Common/pbkdf2.d \
./Application/src/Common/se3_common.d \
./Application/src/Common/sha256.d 


# Each subdirectory must supply rules for building sources it contributes
Application/src/Common/aes256.o: /Users/Nico/Documents/SeCube_Project/Code/Optimized_APIs_Firmware_NonBlockingLogin_newDebug/SEcube_SDK/Development/Environment/secube/src/Common/aes256.c
	@echo 'Building file: $<'
	@echo 'Invoking: MCU GCC Compiler'
	@echo $(PWD)
	arm-none-eabi-gcc -mcpu=cortex-m4 -mthumb -mfloat-abi=hard -mfpu=fpv4-sp-d16 -std=c99 '-D__weak=__attribute__((weak))' '-D__packed="__attribute__((__packed__))"' -DUSE_HAL_DRIVER -DSTM32F429xx -I"/Users/Nico/Documents/SeCube_Project/Code/Optimized_APIs_Firmware_NonBlockingLogin_newDebug/SEcube_SDK/Development/Environment/Drivers/STM32F4xx_HAL_Driver/Inc" -I"/Users/Nico/Documents/SeCube_Project/Code/Optimized_APIs_Firmware_NonBlockingLogin_newDebug/SEcube_SDK/Development/Environment/Drivers/STM32F4xx_HAL_Driver/Inc/Legacy" -I"/Users/Nico/Documents/SeCube_Project/Code/Optimized_APIs_Firmware_NonBlockingLogin_newDebug/SEcube_SDK/Development/Environment/Middlewares/ST/STM32_USB_Device_Library/Core/Inc" -I"/Users/Nico/Documents/SeCube_Project/Code/Optimized_APIs_Firmware_NonBlockingLogin_newDebug/SEcube_SDK/Development/Environment/Middlewares/ST/STM32_USB_Device_Library/Class/MSC/Inc" -I"/Users/Nico/Documents/SeCube_Project/Code/Optimized_APIs_Firmware_NonBlockingLogin_newDebug/SEcube_SDK/Development/Environment/Drivers/CMSIS/Include" -I"/Users/Nico/Documents/SeCube_Project/Code/Optimized_APIs_Firmware_NonBlockingLogin_newDebug/SEcube_SDK/Development/Environment/Drivers/CMSIS/Device/ST/STM32F4xx/Include" -I"/Users/Nico/Documents/SeCube_Project/Code/Optimized_APIs_Firmware_NonBlockingLogin_newDebug/SEcube_SDK/Development/Environment/secube/src" -I"/Users/Nico/Documents/SeCube_Project/Code/Optimized_APIs_Firmware_NonBlockingLogin_newDebug/SEcube_SDK/Development/Environment/secube/src/Common" -I"/Users/Nico/Documents/SeCube_Project/Code/Optimized_APIs_Firmware_NonBlockingLogin_newDebug/SEcube_SDK/Development/Environment/secube/src/Device"  -O0 -g3 -Wall -fmessage-length=0 -ffunction-sections -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

Application/src/Common/crc16.o: /Users/Nico/Documents/SeCube_Project/Code/Optimized_APIs_Firmware_NonBlockingLogin_newDebug/SEcube_SDK/Development/Environment/secube/src/Common/crc16.c
	@echo 'Building file: $<'
	@echo 'Invoking: MCU GCC Compiler'
	@echo $(PWD)
	arm-none-eabi-gcc -mcpu=cortex-m4 -mthumb -mfloat-abi=hard -mfpu=fpv4-sp-d16 -std=c99 '-D__weak=__attribute__((weak))' '-D__packed="__attribute__((__packed__))"' -DUSE_HAL_DRIVER -DSTM32F429xx -I"/Users/Nico/Documents/SeCube_Project/Code/Optimized_APIs_Firmware_NonBlockingLogin_newDebug/SEcube_SDK/Development/Environment/Drivers/STM32F4xx_HAL_Driver/Inc" -I"/Users/Nico/Documents/SeCube_Project/Code/Optimized_APIs_Firmware_NonBlockingLogin_newDebug/SEcube_SDK/Development/Environment/Drivers/STM32F4xx_HAL_Driver/Inc/Legacy" -I"/Users/Nico/Documents/SeCube_Project/Code/Optimized_APIs_Firmware_NonBlockingLogin_newDebug/SEcube_SDK/Development/Environment/Middlewares/ST/STM32_USB_Device_Library/Core/Inc" -I"/Users/Nico/Documents/SeCube_Project/Code/Optimized_APIs_Firmware_NonBlockingLogin_newDebug/SEcube_SDK/Development/Environment/Middlewares/ST/STM32_USB_Device_Library/Class/MSC/Inc" -I"/Users/Nico/Documents/SeCube_Project/Code/Optimized_APIs_Firmware_NonBlockingLogin_newDebug/SEcube_SDK/Development/Environment/Drivers/CMSIS/Include" -I"/Users/Nico/Documents/SeCube_Project/Code/Optimized_APIs_Firmware_NonBlockingLogin_newDebug/SEcube_SDK/Development/Environment/Drivers/CMSIS/Device/ST/STM32F4xx/Include" -I"/Users/Nico/Documents/SeCube_Project/Code/Optimized_APIs_Firmware_NonBlockingLogin_newDebug/SEcube_SDK/Development/Environment/secube/src" -I"/Users/Nico/Documents/SeCube_Project/Code/Optimized_APIs_Firmware_NonBlockingLogin_newDebug/SEcube_SDK/Development/Environment/secube/src/Common" -I"/Users/Nico/Documents/SeCube_Project/Code/Optimized_APIs_Firmware_NonBlockingLogin_newDebug/SEcube_SDK/Development/Environment/secube/src/Device"  -O0 -g3 -Wall -fmessage-length=0 -ffunction-sections -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

Application/src/Common/pbkdf2.o: /Users/Nico/Documents/SeCube_Project/Code/Optimized_APIs_Firmware_NonBlockingLogin_newDebug/SEcube_SDK/Development/Environment/secube/src/Common/pbkdf2.c
	@echo 'Building file: $<'
	@echo 'Invoking: MCU GCC Compiler'
	@echo $(PWD)
	arm-none-eabi-gcc -mcpu=cortex-m4 -mthumb -mfloat-abi=hard -mfpu=fpv4-sp-d16 -std=c99 '-D__weak=__attribute__((weak))' '-D__packed="__attribute__((__packed__))"' -DUSE_HAL_DRIVER -DSTM32F429xx -I"/Users/Nico/Documents/SeCube_Project/Code/Optimized_APIs_Firmware_NonBlockingLogin_newDebug/SEcube_SDK/Development/Environment/Drivers/STM32F4xx_HAL_Driver/Inc" -I"/Users/Nico/Documents/SeCube_Project/Code/Optimized_APIs_Firmware_NonBlockingLogin_newDebug/SEcube_SDK/Development/Environment/Drivers/STM32F4xx_HAL_Driver/Inc/Legacy" -I"/Users/Nico/Documents/SeCube_Project/Code/Optimized_APIs_Firmware_NonBlockingLogin_newDebug/SEcube_SDK/Development/Environment/Middlewares/ST/STM32_USB_Device_Library/Core/Inc" -I"/Users/Nico/Documents/SeCube_Project/Code/Optimized_APIs_Firmware_NonBlockingLogin_newDebug/SEcube_SDK/Development/Environment/Middlewares/ST/STM32_USB_Device_Library/Class/MSC/Inc" -I"/Users/Nico/Documents/SeCube_Project/Code/Optimized_APIs_Firmware_NonBlockingLogin_newDebug/SEcube_SDK/Development/Environment/Drivers/CMSIS/Include" -I"/Users/Nico/Documents/SeCube_Project/Code/Optimized_APIs_Firmware_NonBlockingLogin_newDebug/SEcube_SDK/Development/Environment/Drivers/CMSIS/Device/ST/STM32F4xx/Include" -I"/Users/Nico/Documents/SeCube_Project/Code/Optimized_APIs_Firmware_NonBlockingLogin_newDebug/SEcube_SDK/Development/Environment/secube/src" -I"/Users/Nico/Documents/SeCube_Project/Code/Optimized_APIs_Firmware_NonBlockingLogin_newDebug/SEcube_SDK/Development/Environment/secube/src/Common" -I"/Users/Nico/Documents/SeCube_Project/Code/Optimized_APIs_Firmware_NonBlockingLogin_newDebug/SEcube_SDK/Development/Environment/secube/src/Device"  -O0 -g3 -Wall -fmessage-length=0 -ffunction-sections -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

Application/src/Common/se3_common.o: /Users/Nico/Documents/SeCube_Project/Code/Optimized_APIs_Firmware_NonBlockingLogin_newDebug/SEcube_SDK/Development/Environment/secube/src/Common/se3_common.c
	@echo 'Building file: $<'
	@echo 'Invoking: MCU GCC Compiler'
	@echo $(PWD)
	arm-none-eabi-gcc -mcpu=cortex-m4 -mthumb -mfloat-abi=hard -mfpu=fpv4-sp-d16 -std=c99 '-D__weak=__attribute__((weak))' '-D__packed="__attribute__((__packed__))"' -DUSE_HAL_DRIVER -DSTM32F429xx -I"/Users/Nico/Documents/SeCube_Project/Code/Optimized_APIs_Firmware_NonBlockingLogin_newDebug/SEcube_SDK/Development/Environment/Drivers/STM32F4xx_HAL_Driver/Inc" -I"/Users/Nico/Documents/SeCube_Project/Code/Optimized_APIs_Firmware_NonBlockingLogin_newDebug/SEcube_SDK/Development/Environment/Drivers/STM32F4xx_HAL_Driver/Inc/Legacy" -I"/Users/Nico/Documents/SeCube_Project/Code/Optimized_APIs_Firmware_NonBlockingLogin_newDebug/SEcube_SDK/Development/Environment/Middlewares/ST/STM32_USB_Device_Library/Core/Inc" -I"/Users/Nico/Documents/SeCube_Project/Code/Optimized_APIs_Firmware_NonBlockingLogin_newDebug/SEcube_SDK/Development/Environment/Middlewares/ST/STM32_USB_Device_Library/Class/MSC/Inc" -I"/Users/Nico/Documents/SeCube_Project/Code/Optimized_APIs_Firmware_NonBlockingLogin_newDebug/SEcube_SDK/Development/Environment/Drivers/CMSIS/Include" -I"/Users/Nico/Documents/SeCube_Project/Code/Optimized_APIs_Firmware_NonBlockingLogin_newDebug/SEcube_SDK/Development/Environment/Drivers/CMSIS/Device/ST/STM32F4xx/Include" -I"/Users/Nico/Documents/SeCube_Project/Code/Optimized_APIs_Firmware_NonBlockingLogin_newDebug/SEcube_SDK/Development/Environment/secube/src" -I"/Users/Nico/Documents/SeCube_Project/Code/Optimized_APIs_Firmware_NonBlockingLogin_newDebug/SEcube_SDK/Development/Environment/secube/src/Common" -I"/Users/Nico/Documents/SeCube_Project/Code/Optimized_APIs_Firmware_NonBlockingLogin_newDebug/SEcube_SDK/Development/Environment/secube/src/Device"  -O0 -g3 -Wall -fmessage-length=0 -ffunction-sections -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

Application/src/Common/sha256.o: /Users/Nico/Documents/SeCube_Project/Code/Optimized_APIs_Firmware_NonBlockingLogin_newDebug/SEcube_SDK/Development/Environment/secube/src/Common/sha256.c
	@echo 'Building file: $<'
	@echo 'Invoking: MCU GCC Compiler'
	@echo $(PWD)
	arm-none-eabi-gcc -mcpu=cortex-m4 -mthumb -mfloat-abi=hard -mfpu=fpv4-sp-d16 -std=c99 '-D__weak=__attribute__((weak))' '-D__packed="__attribute__((__packed__))"' -DUSE_HAL_DRIVER -DSTM32F429xx -I"/Users/Nico/Documents/SeCube_Project/Code/Optimized_APIs_Firmware_NonBlockingLogin_newDebug/SEcube_SDK/Development/Environment/Drivers/STM32F4xx_HAL_Driver/Inc" -I"/Users/Nico/Documents/SeCube_Project/Code/Optimized_APIs_Firmware_NonBlockingLogin_newDebug/SEcube_SDK/Development/Environment/Drivers/STM32F4xx_HAL_Driver/Inc/Legacy" -I"/Users/Nico/Documents/SeCube_Project/Code/Optimized_APIs_Firmware_NonBlockingLogin_newDebug/SEcube_SDK/Development/Environment/Middlewares/ST/STM32_USB_Device_Library/Core/Inc" -I"/Users/Nico/Documents/SeCube_Project/Code/Optimized_APIs_Firmware_NonBlockingLogin_newDebug/SEcube_SDK/Development/Environment/Middlewares/ST/STM32_USB_Device_Library/Class/MSC/Inc" -I"/Users/Nico/Documents/SeCube_Project/Code/Optimized_APIs_Firmware_NonBlockingLogin_newDebug/SEcube_SDK/Development/Environment/Drivers/CMSIS/Include" -I"/Users/Nico/Documents/SeCube_Project/Code/Optimized_APIs_Firmware_NonBlockingLogin_newDebug/SEcube_SDK/Development/Environment/Drivers/CMSIS/Device/ST/STM32F4xx/Include" -I"/Users/Nico/Documents/SeCube_Project/Code/Optimized_APIs_Firmware_NonBlockingLogin_newDebug/SEcube_SDK/Development/Environment/secube/src" -I"/Users/Nico/Documents/SeCube_Project/Code/Optimized_APIs_Firmware_NonBlockingLogin_newDebug/SEcube_SDK/Development/Environment/secube/src/Common" -I"/Users/Nico/Documents/SeCube_Project/Code/Optimized_APIs_Firmware_NonBlockingLogin_newDebug/SEcube_SDK/Development/Environment/secube/src/Device"  -O0 -g3 -Wall -fmessage-length=0 -ffunction-sections -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '



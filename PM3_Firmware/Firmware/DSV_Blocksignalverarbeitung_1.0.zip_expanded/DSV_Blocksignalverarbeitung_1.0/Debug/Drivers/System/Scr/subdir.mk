################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (12.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
S_SRCS += \
../Drivers/System/Scr/startup_stm32f769xx.s 

C_SRCS += \
../Drivers/System/Scr/stm32f7xx_it.c \
../Drivers/System/Scr/syscalls.c \
../Drivers/System/Scr/system_stm32f7xx.c \
../Drivers/System/Scr/usbh_conf.c 

OBJS += \
./Drivers/System/Scr/startup_stm32f769xx.o \
./Drivers/System/Scr/stm32f7xx_it.o \
./Drivers/System/Scr/syscalls.o \
./Drivers/System/Scr/system_stm32f7xx.o \
./Drivers/System/Scr/usbh_conf.o 

S_DEPS += \
./Drivers/System/Scr/startup_stm32f769xx.d 

C_DEPS += \
./Drivers/System/Scr/stm32f7xx_it.d \
./Drivers/System/Scr/syscalls.d \
./Drivers/System/Scr/system_stm32f7xx.d \
./Drivers/System/Scr/usbh_conf.d 


# Each subdirectory must supply rules for building sources it contributes
Drivers/System/Scr/%.o: ../Drivers/System/Scr/%.s Drivers/System/Scr/subdir.mk
	arm-none-eabi-gcc -mcpu=cortex-m7 -g3 -c -x assembler-with-cpp -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv5-d16 -mfloat-abi=hard -mthumb -o "$@" "$<"
Drivers/System/Scr/%.o Drivers/System/Scr/%.su Drivers/System/Scr/%.cyclo: ../Drivers/System/Scr/%.c Drivers/System/Scr/subdir.mk
	arm-none-eabi-gcc -mfloat-abi=hard "$<" -mcpu=cortex-m7 -std=gnu11 -g3 -DUSE_HAL_DRIVER -DSTM32F769xx -DUSE_STM32F769I_DISCO -DUSE_USB_HS -c -O0 -ffunction-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv5-d16 -mfloat-abi=hard -mthumb -o "$@" @"Drivers/System/Scr/stm32f7xx_it.c_includes.args"

clean: clean-Drivers-2f-System-2f-Scr

clean-Drivers-2f-System-2f-Scr:
	-$(RM) ./Drivers/System/Scr/startup_stm32f769xx.d ./Drivers/System/Scr/startup_stm32f769xx.o ./Drivers/System/Scr/stm32f7xx_it.cyclo ./Drivers/System/Scr/stm32f7xx_it.d ./Drivers/System/Scr/stm32f7xx_it.o ./Drivers/System/Scr/stm32f7xx_it.su ./Drivers/System/Scr/syscalls.cyclo ./Drivers/System/Scr/syscalls.d ./Drivers/System/Scr/syscalls.o ./Drivers/System/Scr/syscalls.su ./Drivers/System/Scr/system_stm32f7xx.cyclo ./Drivers/System/Scr/system_stm32f7xx.d ./Drivers/System/Scr/system_stm32f7xx.o ./Drivers/System/Scr/system_stm32f7xx.su ./Drivers/System/Scr/usbh_conf.cyclo ./Drivers/System/Scr/usbh_conf.d ./Drivers/System/Scr/usbh_conf.o ./Drivers/System/Scr/usbh_conf.su

.PHONY: clean-Drivers-2f-System-2f-Scr


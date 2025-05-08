################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (12.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Drivers/Components/wm8994/wm8994.c 

OBJS += \
./Drivers/Components/wm8994/wm8994.o 

C_DEPS += \
./Drivers/Components/wm8994/wm8994.d 


# Each subdirectory must supply rules for building sources it contributes
Drivers/Components/wm8994/%.o Drivers/Components/wm8994/%.su Drivers/Components/wm8994/%.cyclo: ../Drivers/Components/wm8994/%.c Drivers/Components/wm8994/subdir.mk
	arm-none-eabi-gcc -mfloat-abi=hard "$<" -mcpu=cortex-m7 -std=gnu11 -g3 -DUSE_HAL_DRIVER -DSTM32F769xx -DUSE_STM32F769I_DISCO -DUSE_USB_HS -c -O0 -ffunction-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv5-d16 -mfloat-abi=hard -mthumb -o "$@" @"Drivers/Components/wm8994/wm8994.c_includes.args"

clean: clean-Drivers-2f-Components-2f-wm8994

clean-Drivers-2f-Components-2f-wm8994:
	-$(RM) ./Drivers/Components/wm8994/wm8994.cyclo ./Drivers/Components/wm8994/wm8994.d ./Drivers/Components/wm8994/wm8994.o ./Drivers/Components/wm8994/wm8994.su

.PHONY: clean-Drivers-2f-Components-2f-wm8994


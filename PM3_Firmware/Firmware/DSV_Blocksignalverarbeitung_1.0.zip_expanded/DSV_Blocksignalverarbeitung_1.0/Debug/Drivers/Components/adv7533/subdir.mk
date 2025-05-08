################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (12.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Drivers/Components/adv7533/adv7533.c 

OBJS += \
./Drivers/Components/adv7533/adv7533.o 

C_DEPS += \
./Drivers/Components/adv7533/adv7533.d 


# Each subdirectory must supply rules for building sources it contributes
Drivers/Components/adv7533/%.o Drivers/Components/adv7533/%.su Drivers/Components/adv7533/%.cyclo: ../Drivers/Components/adv7533/%.c Drivers/Components/adv7533/subdir.mk
	arm-none-eabi-gcc -mfloat-abi=hard "$<" -mcpu=cortex-m7 -std=gnu11 -g3 -DUSE_HAL_DRIVER -DSTM32F769xx -DUSE_STM32F769I_DISCO -DUSE_USB_HS -c -O0 -ffunction-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv5-d16 -mfloat-abi=hard -mthumb -o "$@" @"Drivers/Components/adv7533/adv7533.c_includes.args"

clean: clean-Drivers-2f-Components-2f-adv7533

clean-Drivers-2f-Components-2f-adv7533:
	-$(RM) ./Drivers/Components/adv7533/adv7533.cyclo ./Drivers/Components/adv7533/adv7533.d ./Drivers/Components/adv7533/adv7533.o ./Drivers/Components/adv7533/adv7533.su

.PHONY: clean-Drivers-2f-Components-2f-adv7533


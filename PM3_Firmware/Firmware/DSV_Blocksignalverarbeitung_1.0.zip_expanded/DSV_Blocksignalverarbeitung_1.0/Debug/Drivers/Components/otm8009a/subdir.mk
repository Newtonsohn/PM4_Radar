################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (12.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Drivers/Components/otm8009a/otm8009a.c 

OBJS += \
./Drivers/Components/otm8009a/otm8009a.o 

C_DEPS += \
./Drivers/Components/otm8009a/otm8009a.d 


# Each subdirectory must supply rules for building sources it contributes
Drivers/Components/otm8009a/%.o Drivers/Components/otm8009a/%.su Drivers/Components/otm8009a/%.cyclo: ../Drivers/Components/otm8009a/%.c Drivers/Components/otm8009a/subdir.mk
	arm-none-eabi-gcc -mfloat-abi=hard "$<" -mcpu=cortex-m7 -std=gnu11 -g3 -DUSE_HAL_DRIVER -DSTM32F769xx -DUSE_STM32F769I_DISCO -DUSE_USB_HS -c -O0 -ffunction-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv5-d16 -mfloat-abi=hard -mthumb -o "$@" @"Drivers/Components/otm8009a/otm8009a.c_includes.args"

clean: clean-Drivers-2f-Components-2f-otm8009a

clean-Drivers-2f-Components-2f-otm8009a:
	-$(RM) ./Drivers/Components/otm8009a/otm8009a.cyclo ./Drivers/Components/otm8009a/otm8009a.d ./Drivers/Components/otm8009a/otm8009a.o ./Drivers/Components/otm8009a/otm8009a.su

.PHONY: clean-Drivers-2f-Components-2f-otm8009a


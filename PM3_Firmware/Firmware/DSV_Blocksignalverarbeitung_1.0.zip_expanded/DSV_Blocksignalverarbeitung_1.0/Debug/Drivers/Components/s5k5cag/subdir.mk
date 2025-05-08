################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (12.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Drivers/Components/s5k5cag/s5k5cag.c 

OBJS += \
./Drivers/Components/s5k5cag/s5k5cag.o 

C_DEPS += \
./Drivers/Components/s5k5cag/s5k5cag.d 


# Each subdirectory must supply rules for building sources it contributes
Drivers/Components/s5k5cag/%.o Drivers/Components/s5k5cag/%.su Drivers/Components/s5k5cag/%.cyclo: ../Drivers/Components/s5k5cag/%.c Drivers/Components/s5k5cag/subdir.mk
	arm-none-eabi-gcc -mfloat-abi=hard "$<" -mcpu=cortex-m7 -std=gnu11 -g3 -DUSE_HAL_DRIVER -DSTM32F769xx -DUSE_STM32F769I_DISCO -DUSE_USB_HS -c -O0 -ffunction-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv5-d16 -mfloat-abi=hard -mthumb -o "$@" @"Drivers/Components/s5k5cag/s5k5cag.c_includes.args"

clean: clean-Drivers-2f-Components-2f-s5k5cag

clean-Drivers-2f-Components-2f-s5k5cag:
	-$(RM) ./Drivers/Components/s5k5cag/s5k5cag.cyclo ./Drivers/Components/s5k5cag/s5k5cag.d ./Drivers/Components/s5k5cag/s5k5cag.o ./Drivers/Components/s5k5cag/s5k5cag.su

.PHONY: clean-Drivers-2f-Components-2f-s5k5cag


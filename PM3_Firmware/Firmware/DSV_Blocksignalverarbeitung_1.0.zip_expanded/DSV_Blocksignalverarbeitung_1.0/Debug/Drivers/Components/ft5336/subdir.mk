################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (12.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Drivers/Components/ft5336/ft5336.c 

OBJS += \
./Drivers/Components/ft5336/ft5336.o 

C_DEPS += \
./Drivers/Components/ft5336/ft5336.d 


# Each subdirectory must supply rules for building sources it contributes
Drivers/Components/ft5336/%.o Drivers/Components/ft5336/%.su Drivers/Components/ft5336/%.cyclo: ../Drivers/Components/ft5336/%.c Drivers/Components/ft5336/subdir.mk
	arm-none-eabi-gcc -mfloat-abi=hard "$<" -mcpu=cortex-m7 -std=gnu11 -g3 -DUSE_HAL_DRIVER -DSTM32F769xx -DUSE_STM32F769I_DISCO -DUSE_USB_HS -c -O0 -ffunction-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv5-d16 -mfloat-abi=hard -mthumb -o "$@" @"Drivers/Components/ft5336/ft5336.c_includes.args"

clean: clean-Drivers-2f-Components-2f-ft5336

clean-Drivers-2f-Components-2f-ft5336:
	-$(RM) ./Drivers/Components/ft5336/ft5336.cyclo ./Drivers/Components/ft5336/ft5336.d ./Drivers/Components/ft5336/ft5336.o ./Drivers/Components/ft5336/ft5336.su

.PHONY: clean-Drivers-2f-Components-2f-ft5336


################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (12.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Drivers/Components/st7735/st7735.c 

OBJS += \
./Drivers/Components/st7735/st7735.o 

C_DEPS += \
./Drivers/Components/st7735/st7735.d 


# Each subdirectory must supply rules for building sources it contributes
Drivers/Components/st7735/%.o Drivers/Components/st7735/%.su Drivers/Components/st7735/%.cyclo: ../Drivers/Components/st7735/%.c Drivers/Components/st7735/subdir.mk
	arm-none-eabi-gcc -mfloat-abi=hard "$<" -mcpu=cortex-m7 -std=gnu11 -g3 -DUSE_HAL_DRIVER -DSTM32F769xx -DUSE_STM32F769I_DISCO -DUSE_USB_HS -c -O0 -ffunction-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv5-d16 -mfloat-abi=hard -mthumb -o "$@" @"Drivers/Components/st7735/st7735.c_includes.args"

clean: clean-Drivers-2f-Components-2f-st7735

clean-Drivers-2f-Components-2f-st7735:
	-$(RM) ./Drivers/Components/st7735/st7735.cyclo ./Drivers/Components/st7735/st7735.d ./Drivers/Components/st7735/st7735.o ./Drivers/Components/st7735/st7735.su

.PHONY: clean-Drivers-2f-Components-2f-st7735


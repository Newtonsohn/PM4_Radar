################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (13.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Drivers/BSP/Components/lsm303dlhc/lsm303dlhc.c 

OBJS += \
./Drivers/BSP/Components/lsm303dlhc/lsm303dlhc.o 

C_DEPS += \
./Drivers/BSP/Components/lsm303dlhc/lsm303dlhc.d 


# Each subdirectory must supply rules for building sources it contributes
Drivers/BSP/Components/lsm303dlhc/%.o Drivers/BSP/Components/lsm303dlhc/%.su Drivers/BSP/Components/lsm303dlhc/%.cyclo: ../Drivers/BSP/Components/lsm303dlhc/%.c Drivers/BSP/Components/lsm303dlhc/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DUSE_HAL_DRIVER -DDEBUG -DSTM32F429xx -DARM_MATH_CM4 -c -I../Core/Inc -I../Drivers/CMSIS/Include -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I"../Drivers/BSP/Components/Common" -I"../Drivers/BSP/Components/stmpe811" -I"../Drivers/BSP/Components/ili9341" -I"../Drivers/BSP/STM32F429I-Discovery" -I"../Utilities/Fonts" -I../Drivers/CMSIS/DSP/Include -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Drivers-2f-BSP-2f-Components-2f-lsm303dlhc

clean-Drivers-2f-BSP-2f-Components-2f-lsm303dlhc:
	-$(RM) ./Drivers/BSP/Components/lsm303dlhc/lsm303dlhc.cyclo ./Drivers/BSP/Components/lsm303dlhc/lsm303dlhc.d ./Drivers/BSP/Components/lsm303dlhc/lsm303dlhc.o ./Drivers/BSP/Components/lsm303dlhc/lsm303dlhc.su

.PHONY: clean-Drivers-2f-BSP-2f-Components-2f-lsm303dlhc


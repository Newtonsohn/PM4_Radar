################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (12.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Drivers/STM32F769I-Discovery/stm32f769i_discovery.c \
../Drivers/STM32F769I-Discovery/stm32f769i_discovery_audio.c \
../Drivers/STM32F769I-Discovery/stm32f769i_discovery_eeprom.c \
../Drivers/STM32F769I-Discovery/stm32f769i_discovery_lcd.c \
../Drivers/STM32F769I-Discovery/stm32f769i_discovery_qspi.c \
../Drivers/STM32F769I-Discovery/stm32f769i_discovery_sd.c \
../Drivers/STM32F769I-Discovery/stm32f769i_discovery_sdram.c \
../Drivers/STM32F769I-Discovery/stm32f769i_discovery_ts.c 

OBJS += \
./Drivers/STM32F769I-Discovery/stm32f769i_discovery.o \
./Drivers/STM32F769I-Discovery/stm32f769i_discovery_audio.o \
./Drivers/STM32F769I-Discovery/stm32f769i_discovery_eeprom.o \
./Drivers/STM32F769I-Discovery/stm32f769i_discovery_lcd.o \
./Drivers/STM32F769I-Discovery/stm32f769i_discovery_qspi.o \
./Drivers/STM32F769I-Discovery/stm32f769i_discovery_sd.o \
./Drivers/STM32F769I-Discovery/stm32f769i_discovery_sdram.o \
./Drivers/STM32F769I-Discovery/stm32f769i_discovery_ts.o 

C_DEPS += \
./Drivers/STM32F769I-Discovery/stm32f769i_discovery.d \
./Drivers/STM32F769I-Discovery/stm32f769i_discovery_audio.d \
./Drivers/STM32F769I-Discovery/stm32f769i_discovery_eeprom.d \
./Drivers/STM32F769I-Discovery/stm32f769i_discovery_lcd.d \
./Drivers/STM32F769I-Discovery/stm32f769i_discovery_qspi.d \
./Drivers/STM32F769I-Discovery/stm32f769i_discovery_sd.d \
./Drivers/STM32F769I-Discovery/stm32f769i_discovery_sdram.d \
./Drivers/STM32F769I-Discovery/stm32f769i_discovery_ts.d 


# Each subdirectory must supply rules for building sources it contributes
Drivers/STM32F769I-Discovery/%.o Drivers/STM32F769I-Discovery/%.su Drivers/STM32F769I-Discovery/%.cyclo: ../Drivers/STM32F769I-Discovery/%.c Drivers/STM32F769I-Discovery/subdir.mk
	arm-none-eabi-gcc -mfloat-abi=hard "$<" -mcpu=cortex-m7 -std=gnu11 -g3 -DUSE_HAL_DRIVER -DSTM32F769xx -DUSE_STM32F769I_DISCO -DUSE_USB_HS -c -O0 -ffunction-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv5-d16 -mfloat-abi=hard -mthumb -o "$@" @"Drivers/STM32F769I-Discovery/stm32f769i_discovery.c_includes.args"

clean: clean-Drivers-2f-STM32F769I-2d-Discovery

clean-Drivers-2f-STM32F769I-2d-Discovery:
	-$(RM) ./Drivers/STM32F769I-Discovery/stm32f769i_discovery.cyclo ./Drivers/STM32F769I-Discovery/stm32f769i_discovery.d ./Drivers/STM32F769I-Discovery/stm32f769i_discovery.o ./Drivers/STM32F769I-Discovery/stm32f769i_discovery.su ./Drivers/STM32F769I-Discovery/stm32f769i_discovery_audio.cyclo ./Drivers/STM32F769I-Discovery/stm32f769i_discovery_audio.d ./Drivers/STM32F769I-Discovery/stm32f769i_discovery_audio.o ./Drivers/STM32F769I-Discovery/stm32f769i_discovery_audio.su ./Drivers/STM32F769I-Discovery/stm32f769i_discovery_eeprom.cyclo ./Drivers/STM32F769I-Discovery/stm32f769i_discovery_eeprom.d ./Drivers/STM32F769I-Discovery/stm32f769i_discovery_eeprom.o ./Drivers/STM32F769I-Discovery/stm32f769i_discovery_eeprom.su ./Drivers/STM32F769I-Discovery/stm32f769i_discovery_lcd.cyclo ./Drivers/STM32F769I-Discovery/stm32f769i_discovery_lcd.d ./Drivers/STM32F769I-Discovery/stm32f769i_discovery_lcd.o ./Drivers/STM32F769I-Discovery/stm32f769i_discovery_lcd.su ./Drivers/STM32F769I-Discovery/stm32f769i_discovery_qspi.cyclo ./Drivers/STM32F769I-Discovery/stm32f769i_discovery_qspi.d ./Drivers/STM32F769I-Discovery/stm32f769i_discovery_qspi.o ./Drivers/STM32F769I-Discovery/stm32f769i_discovery_qspi.su ./Drivers/STM32F769I-Discovery/stm32f769i_discovery_sd.cyclo ./Drivers/STM32F769I-Discovery/stm32f769i_discovery_sd.d ./Drivers/STM32F769I-Discovery/stm32f769i_discovery_sd.o ./Drivers/STM32F769I-Discovery/stm32f769i_discovery_sd.su ./Drivers/STM32F769I-Discovery/stm32f769i_discovery_sdram.cyclo ./Drivers/STM32F769I-Discovery/stm32f769i_discovery_sdram.d ./Drivers/STM32F769I-Discovery/stm32f769i_discovery_sdram.o ./Drivers/STM32F769I-Discovery/stm32f769i_discovery_sdram.su ./Drivers/STM32F769I-Discovery/stm32f769i_discovery_ts.cyclo ./Drivers/STM32F769I-Discovery/stm32f769i_discovery_ts.d ./Drivers/STM32F769I-Discovery/stm32f769i_discovery_ts.o ./Drivers/STM32F769I-Discovery/stm32f769i_discovery_ts.su

.PHONY: clean-Drivers-2f-STM32F769I-2d-Discovery


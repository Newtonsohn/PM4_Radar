################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (12.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Application/Src/Codec.c \
../Application/Src/GUI.c \
../Application/Src/Processing.c \
../Application/Src/main.c 

OBJS += \
./Application/Src/Codec.o \
./Application/Src/GUI.o \
./Application/Src/Processing.o \
./Application/Src/main.o 

C_DEPS += \
./Application/Src/Codec.d \
./Application/Src/GUI.d \
./Application/Src/Processing.d \
./Application/Src/main.d 


# Each subdirectory must supply rules for building sources it contributes
Application/Src/%.o Application/Src/%.su Application/Src/%.cyclo: ../Application/Src/%.c Application/Src/subdir.mk
	arm-none-eabi-gcc -mfloat-abi=hard "$<" -mcpu=cortex-m7 -std=gnu11 -g3 -DUSE_HAL_DRIVER -DSTM32F769xx -DUSE_STM32F769I_DISCO -DUSE_USB_HS -c -O0 -ffunction-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv5-d16 -mfloat-abi=hard -mthumb -o "$@" @"Application/Src/Codec.c_includes.args"

clean: clean-Application-2f-Src

clean-Application-2f-Src:
	-$(RM) ./Application/Src/Codec.cyclo ./Application/Src/Codec.d ./Application/Src/Codec.o ./Application/Src/Codec.su ./Application/Src/GUI.cyclo ./Application/Src/GUI.d ./Application/Src/GUI.o ./Application/Src/GUI.su ./Application/Src/Processing.cyclo ./Application/Src/Processing.d ./Application/Src/Processing.o ./Application/Src/Processing.su ./Application/Src/main.cyclo ./Application/Src/main.d ./Application/Src/main.o ./Application/Src/main.su

.PHONY: clean-Application-2f-Src


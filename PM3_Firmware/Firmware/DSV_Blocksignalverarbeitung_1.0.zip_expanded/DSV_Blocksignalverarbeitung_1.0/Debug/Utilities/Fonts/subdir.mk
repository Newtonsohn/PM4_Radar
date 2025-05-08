################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (12.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Utilities/Fonts/font12.c \
../Utilities/Fonts/font16.c \
../Utilities/Fonts/font20.c \
../Utilities/Fonts/font24.c \
../Utilities/Fonts/font8.c 

OBJS += \
./Utilities/Fonts/font12.o \
./Utilities/Fonts/font16.o \
./Utilities/Fonts/font20.o \
./Utilities/Fonts/font24.o \
./Utilities/Fonts/font8.o 

C_DEPS += \
./Utilities/Fonts/font12.d \
./Utilities/Fonts/font16.d \
./Utilities/Fonts/font20.d \
./Utilities/Fonts/font24.d \
./Utilities/Fonts/font8.d 


# Each subdirectory must supply rules for building sources it contributes
Utilities/Fonts/%.o Utilities/Fonts/%.su Utilities/Fonts/%.cyclo: ../Utilities/Fonts/%.c Utilities/Fonts/subdir.mk
	arm-none-eabi-gcc -mfloat-abi=hard "$<" -mcpu=cortex-m7 -std=gnu11 -g3 -DUSE_HAL_DRIVER -DSTM32F769xx -DUSE_STM32F769I_DISCO -DUSE_USB_HS -c -O0 -ffunction-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv5-d16 -mfloat-abi=hard -mthumb -o "$@" @"Utilities/Fonts/font12.c_includes.args"

clean: clean-Utilities-2f-Fonts

clean-Utilities-2f-Fonts:
	-$(RM) ./Utilities/Fonts/font12.cyclo ./Utilities/Fonts/font12.d ./Utilities/Fonts/font12.o ./Utilities/Fonts/font12.su ./Utilities/Fonts/font16.cyclo ./Utilities/Fonts/font16.d ./Utilities/Fonts/font16.o ./Utilities/Fonts/font16.su ./Utilities/Fonts/font20.cyclo ./Utilities/Fonts/font20.d ./Utilities/Fonts/font20.o ./Utilities/Fonts/font20.su ./Utilities/Fonts/font24.cyclo ./Utilities/Fonts/font24.d ./Utilities/Fonts/font24.o ./Utilities/Fonts/font24.su ./Utilities/Fonts/font8.cyclo ./Utilities/Fonts/font8.d ./Utilities/Fonts/font8.o ./Utilities/Fonts/font8.su

.PHONY: clean-Utilities-2f-Fonts


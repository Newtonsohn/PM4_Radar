################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (12.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Middlewares/FatFs/option/syscall.c \
../Middlewares/FatFs/option/unicode.c 

OBJS += \
./Middlewares/FatFs/option/syscall.o \
./Middlewares/FatFs/option/unicode.o 

C_DEPS += \
./Middlewares/FatFs/option/syscall.d \
./Middlewares/FatFs/option/unicode.d 


# Each subdirectory must supply rules for building sources it contributes
Middlewares/FatFs/option/%.o Middlewares/FatFs/option/%.su Middlewares/FatFs/option/%.cyclo: ../Middlewares/FatFs/option/%.c Middlewares/FatFs/option/subdir.mk
	arm-none-eabi-gcc -mfloat-abi=hard "$<" -mcpu=cortex-m7 -std=gnu11 -g3 -DUSE_HAL_DRIVER -DSTM32F769xx -DUSE_STM32F769I_DISCO -DUSE_USB_HS -c -O0 -ffunction-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv5-d16 -mfloat-abi=hard -mthumb -o "$@" @"Middlewares/FatFs/option/syscall.c_includes.args"

clean: clean-Middlewares-2f-FatFs-2f-option

clean-Middlewares-2f-FatFs-2f-option:
	-$(RM) ./Middlewares/FatFs/option/syscall.cyclo ./Middlewares/FatFs/option/syscall.d ./Middlewares/FatFs/option/syscall.o ./Middlewares/FatFs/option/syscall.su ./Middlewares/FatFs/option/unicode.cyclo ./Middlewares/FatFs/option/unicode.d ./Middlewares/FatFs/option/unicode.o ./Middlewares/FatFs/option/unicode.su

.PHONY: clean-Middlewares-2f-FatFs-2f-option


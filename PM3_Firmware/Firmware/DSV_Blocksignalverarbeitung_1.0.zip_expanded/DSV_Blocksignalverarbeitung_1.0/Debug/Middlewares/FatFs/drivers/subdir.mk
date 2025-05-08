################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (12.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Middlewares/FatFs/drivers/usbh_diskio.c 

OBJS += \
./Middlewares/FatFs/drivers/usbh_diskio.o 

C_DEPS += \
./Middlewares/FatFs/drivers/usbh_diskio.d 


# Each subdirectory must supply rules for building sources it contributes
Middlewares/FatFs/drivers/%.o Middlewares/FatFs/drivers/%.su Middlewares/FatFs/drivers/%.cyclo: ../Middlewares/FatFs/drivers/%.c Middlewares/FatFs/drivers/subdir.mk
	arm-none-eabi-gcc -mfloat-abi=hard "$<" -mcpu=cortex-m7 -std=gnu11 -g3 -DUSE_HAL_DRIVER -DSTM32F769xx -DUSE_STM32F769I_DISCO -DUSE_USB_HS -c -O0 -ffunction-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv5-d16 -mfloat-abi=hard -mthumb -o "$@" @"Middlewares/FatFs/drivers/usbh_diskio.c_includes.args"

clean: clean-Middlewares-2f-FatFs-2f-drivers

clean-Middlewares-2f-FatFs-2f-drivers:
	-$(RM) ./Middlewares/FatFs/drivers/usbh_diskio.cyclo ./Middlewares/FatFs/drivers/usbh_diskio.d ./Middlewares/FatFs/drivers/usbh_diskio.o ./Middlewares/FatFs/drivers/usbh_diskio.su

.PHONY: clean-Middlewares-2f-FatFs-2f-drivers


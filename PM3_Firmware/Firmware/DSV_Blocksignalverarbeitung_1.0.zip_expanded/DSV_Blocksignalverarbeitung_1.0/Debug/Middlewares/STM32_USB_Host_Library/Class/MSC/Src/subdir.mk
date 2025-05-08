################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (12.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Middlewares/STM32_USB_Host_Library/Class/MSC/Src/usbh_msc.c \
../Middlewares/STM32_USB_Host_Library/Class/MSC/Src/usbh_msc_bot.c \
../Middlewares/STM32_USB_Host_Library/Class/MSC/Src/usbh_msc_scsi.c 

OBJS += \
./Middlewares/STM32_USB_Host_Library/Class/MSC/Src/usbh_msc.o \
./Middlewares/STM32_USB_Host_Library/Class/MSC/Src/usbh_msc_bot.o \
./Middlewares/STM32_USB_Host_Library/Class/MSC/Src/usbh_msc_scsi.o 

C_DEPS += \
./Middlewares/STM32_USB_Host_Library/Class/MSC/Src/usbh_msc.d \
./Middlewares/STM32_USB_Host_Library/Class/MSC/Src/usbh_msc_bot.d \
./Middlewares/STM32_USB_Host_Library/Class/MSC/Src/usbh_msc_scsi.d 


# Each subdirectory must supply rules for building sources it contributes
Middlewares/STM32_USB_Host_Library/Class/MSC/Src/%.o Middlewares/STM32_USB_Host_Library/Class/MSC/Src/%.su Middlewares/STM32_USB_Host_Library/Class/MSC/Src/%.cyclo: ../Middlewares/STM32_USB_Host_Library/Class/MSC/Src/%.c Middlewares/STM32_USB_Host_Library/Class/MSC/Src/subdir.mk
	arm-none-eabi-gcc -mfloat-abi=hard "$<" -mcpu=cortex-m7 -std=gnu11 -g3 -DUSE_HAL_DRIVER -DSTM32F769xx -DUSE_STM32F769I_DISCO -DUSE_USB_HS -c -O0 -ffunction-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv5-d16 -mfloat-abi=hard -mthumb -o "$@" @"Middlewares/STM32_USB_Host_Library/Class/MSC/Src/usbh_msc.c_includes.args"

clean: clean-Middlewares-2f-STM32_USB_Host_Library-2f-Class-2f-MSC-2f-Src

clean-Middlewares-2f-STM32_USB_Host_Library-2f-Class-2f-MSC-2f-Src:
	-$(RM) ./Middlewares/STM32_USB_Host_Library/Class/MSC/Src/usbh_msc.cyclo ./Middlewares/STM32_USB_Host_Library/Class/MSC/Src/usbh_msc.d ./Middlewares/STM32_USB_Host_Library/Class/MSC/Src/usbh_msc.o ./Middlewares/STM32_USB_Host_Library/Class/MSC/Src/usbh_msc.su ./Middlewares/STM32_USB_Host_Library/Class/MSC/Src/usbh_msc_bot.cyclo ./Middlewares/STM32_USB_Host_Library/Class/MSC/Src/usbh_msc_bot.d ./Middlewares/STM32_USB_Host_Library/Class/MSC/Src/usbh_msc_bot.o ./Middlewares/STM32_USB_Host_Library/Class/MSC/Src/usbh_msc_bot.su ./Middlewares/STM32_USB_Host_Library/Class/MSC/Src/usbh_msc_scsi.cyclo ./Middlewares/STM32_USB_Host_Library/Class/MSC/Src/usbh_msc_scsi.d ./Middlewares/STM32_USB_Host_Library/Class/MSC/Src/usbh_msc_scsi.o ./Middlewares/STM32_USB_Host_Library/Class/MSC/Src/usbh_msc_scsi.su

.PHONY: clean-Middlewares-2f-STM32_USB_Host_Library-2f-Class-2f-MSC-2f-Src


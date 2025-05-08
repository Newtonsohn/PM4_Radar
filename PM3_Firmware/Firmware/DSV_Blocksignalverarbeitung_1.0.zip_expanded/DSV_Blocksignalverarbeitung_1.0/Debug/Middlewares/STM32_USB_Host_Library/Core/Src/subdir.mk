################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (12.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Middlewares/STM32_USB_Host_Library/Core/Src/usbh_core.c \
../Middlewares/STM32_USB_Host_Library/Core/Src/usbh_ctlreq.c \
../Middlewares/STM32_USB_Host_Library/Core/Src/usbh_ioreq.c \
../Middlewares/STM32_USB_Host_Library/Core/Src/usbh_pipes.c 

OBJS += \
./Middlewares/STM32_USB_Host_Library/Core/Src/usbh_core.o \
./Middlewares/STM32_USB_Host_Library/Core/Src/usbh_ctlreq.o \
./Middlewares/STM32_USB_Host_Library/Core/Src/usbh_ioreq.o \
./Middlewares/STM32_USB_Host_Library/Core/Src/usbh_pipes.o 

C_DEPS += \
./Middlewares/STM32_USB_Host_Library/Core/Src/usbh_core.d \
./Middlewares/STM32_USB_Host_Library/Core/Src/usbh_ctlreq.d \
./Middlewares/STM32_USB_Host_Library/Core/Src/usbh_ioreq.d \
./Middlewares/STM32_USB_Host_Library/Core/Src/usbh_pipes.d 


# Each subdirectory must supply rules for building sources it contributes
Middlewares/STM32_USB_Host_Library/Core/Src/%.o Middlewares/STM32_USB_Host_Library/Core/Src/%.su Middlewares/STM32_USB_Host_Library/Core/Src/%.cyclo: ../Middlewares/STM32_USB_Host_Library/Core/Src/%.c Middlewares/STM32_USB_Host_Library/Core/Src/subdir.mk
	arm-none-eabi-gcc -mfloat-abi=hard "$<" -mcpu=cortex-m7 -std=gnu11 -g3 -DUSE_HAL_DRIVER -DSTM32F769xx -DUSE_STM32F769I_DISCO -DUSE_USB_HS -c -O0 -ffunction-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv5-d16 -mfloat-abi=hard -mthumb -o "$@" @"Middlewares/STM32_USB_Host_Library/Core/Src/usbh_core.c_includes.args"

clean: clean-Middlewares-2f-STM32_USB_Host_Library-2f-Core-2f-Src

clean-Middlewares-2f-STM32_USB_Host_Library-2f-Core-2f-Src:
	-$(RM) ./Middlewares/STM32_USB_Host_Library/Core/Src/usbh_core.cyclo ./Middlewares/STM32_USB_Host_Library/Core/Src/usbh_core.d ./Middlewares/STM32_USB_Host_Library/Core/Src/usbh_core.o ./Middlewares/STM32_USB_Host_Library/Core/Src/usbh_core.su ./Middlewares/STM32_USB_Host_Library/Core/Src/usbh_ctlreq.cyclo ./Middlewares/STM32_USB_Host_Library/Core/Src/usbh_ctlreq.d ./Middlewares/STM32_USB_Host_Library/Core/Src/usbh_ctlreq.o ./Middlewares/STM32_USB_Host_Library/Core/Src/usbh_ctlreq.su ./Middlewares/STM32_USB_Host_Library/Core/Src/usbh_ioreq.cyclo ./Middlewares/STM32_USB_Host_Library/Core/Src/usbh_ioreq.d ./Middlewares/STM32_USB_Host_Library/Core/Src/usbh_ioreq.o ./Middlewares/STM32_USB_Host_Library/Core/Src/usbh_ioreq.su ./Middlewares/STM32_USB_Host_Library/Core/Src/usbh_pipes.cyclo ./Middlewares/STM32_USB_Host_Library/Core/Src/usbh_pipes.d ./Middlewares/STM32_USB_Host_Library/Core/Src/usbh_pipes.o ./Middlewares/STM32_USB_Host_Library/Core/Src/usbh_pipes.su

.PHONY: clean-Middlewares-2f-STM32_USB_Host_Library-2f-Core-2f-Src


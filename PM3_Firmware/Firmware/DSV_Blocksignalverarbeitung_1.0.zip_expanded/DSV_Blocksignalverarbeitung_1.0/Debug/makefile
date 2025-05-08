################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (12.3.rel1)
################################################################################

-include ../makefile.init

RM := rm -rf

# All of the sources participating in the build are defined here
-include sources.mk
-include Utilities/JPEG/subdir.mk
-include Utilities/Fonts/subdir.mk
-include Middlewares/STM32_USB_Host_Library/Core/Src/subdir.mk
-include Middlewares/STM32_USB_Host_Library/Class/MSC/Src/subdir.mk
-include Middlewares/FatFs/option/subdir.mk
-include Middlewares/FatFs/drivers/subdir.mk
-include Middlewares/FatFs/subdir.mk
-include Drivers/System/Scr/subdir.mk
-include Drivers/STM32F769I-Discovery/subdir.mk
-include Drivers/HAL/Src/subdir.mk
-include Drivers/Components/wm8994/subdir.mk
-include Drivers/Components/ts3510/subdir.mk
-include Drivers/Components/stmpe811/subdir.mk
-include Drivers/Components/st7789h2/subdir.mk
-include Drivers/Components/st7735/subdir.mk
-include Drivers/Components/s5k5cag/subdir.mk
-include Drivers/Components/ov9655/subdir.mk
-include Drivers/Components/otm8009a/subdir.mk
-include Drivers/Components/mfxstm32l152/subdir.mk
-include Drivers/Components/ft6x06/subdir.mk
-include Drivers/Components/ft5336/subdir.mk
-include Drivers/Components/exc7200/subdir.mk
-include Drivers/Components/adv7533/subdir.mk
-include Application/Src/subdir.mk
-include objects.mk

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(S_DEPS)),)
-include $(S_DEPS)
endif
ifneq ($(strip $(S_UPPER_DEPS)),)
-include $(S_UPPER_DEPS)
endif
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
endif

-include ../makefile.defs

OPTIONAL_TOOL_DEPS := \
$(wildcard ../makefile.defs) \
$(wildcard ../makefile.init) \
$(wildcard ../makefile.targets) \


BUILD_ARTIFACT_NAME := STM32F769I_Discovery
BUILD_ARTIFACT_EXTENSION := elf
BUILD_ARTIFACT_PREFIX :=
BUILD_ARTIFACT := $(BUILD_ARTIFACT_PREFIX)$(BUILD_ARTIFACT_NAME)$(if $(BUILD_ARTIFACT_EXTENSION),.$(BUILD_ARTIFACT_EXTENSION),)

# Add inputs and outputs from these tool invocations to the build variables 
EXECUTABLES += \
STM32F769I_Discovery.elf \

MAP_FILES += \
STM32F769I_Discovery.map \

SIZE_OUTPUT += \
default.size.stdout \

OBJDUMP_LIST += \
STM32F769I_Discovery.list \

OBJCOPY_BIN += \
STM32F769I_Discovery.bin \


# All Target
all: main-build

# Main-build Target
main-build: STM32F769I_Discovery.elf secondary-outputs

# Tool invocations
STM32F769I_Discovery.elf STM32F769I_Discovery.map: $(OBJS) $(USER_OBJS) ../STM32F769NIHx_FLASH.ld makefile objects.list $(OPTIONAL_TOOL_DEPS)
	arm-none-eabi-gcc -mfloat-abi=hard -FILL=0xBEEF@unused -o "STM32F769I_Discovery.elf" @"objects.list" $(USER_OBJS) $(LIBS) -mcpu=cortex-m7 -T"../STM32F769NIHx_FLASH.ld" --specs=nosys.specs -Wl,-Map="STM32F769I_Discovery.map" -Wl,--gc-sections -static -L"C:\Users\Lucas\OneDrive\Dokumente\ZHAW Studium Unterlagen\5. Semester\PM3\Project PM3\Firmware\DSV_Blocksignalverarbeitung_1.0.zip_expanded\DSV_Blocksignalverarbeitung_1.0\Drivers\CMSIS" --specs=nano.specs -mfpu=fpv5-d16 -mfloat-abi=hard -mthumb -Wl,--start-group -lc -lm -Wl,--end-group
	@echo 'Finished building target: $@'
	@echo ' '

default.size.stdout: $(EXECUTABLES) makefile objects.list $(OPTIONAL_TOOL_DEPS)
	arm-none-eabi-size  $(EXECUTABLES)
	@echo 'Finished building: $@'
	@echo ' '

STM32F769I_Discovery.list: $(EXECUTABLES) makefile objects.list $(OPTIONAL_TOOL_DEPS)
	arm-none-eabi-objdump -h -S $(EXECUTABLES) > "STM32F769I_Discovery.list"
	@echo 'Finished building: $@'
	@echo ' '

STM32F769I_Discovery.bin: $(EXECUTABLES) makefile objects.list $(OPTIONAL_TOOL_DEPS)
	arm-none-eabi-objcopy  -O binary $(EXECUTABLES) "STM32F769I_Discovery.bin"
	@echo 'Finished building: $@'
	@echo ' '

# Other Targets
clean:
	-$(RM) STM32F769I_Discovery.bin STM32F769I_Discovery.elf STM32F769I_Discovery.list STM32F769I_Discovery.map default.size.stdout
	-@echo ' '

secondary-outputs: $(SIZE_OUTPUT) $(OBJDUMP_LIST) $(OBJCOPY_BIN)

fail-specified-linker-script-missing:
	@echo 'Error: Cannot find the specified linker script. Check the linker settings in the build configuration.'
	@exit 2

warn-no-linker-script-specified:
	@echo 'Warning: No linker script specified. Check the linker settings in the build configuration.'

.PHONY: all clean dependents main-build fail-specified-linker-script-missing warn-no-linker-script-specified

-include ../makefile.targets

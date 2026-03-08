################################################################################
# Automatically-generated file. Do not edit!
################################################################################

SHELL = cmd.exe

CG_TOOL_ROOT := D:/ti/ccsv5/tools/compiler/c6000_7.4.4

ORDERED_OBJS += \
$(GEN_CMDS__FLAG) \
"./ddr/DDR_Init.obj" \
"./edma/memory_transfer.obj" \
"./hyperlink/KeyStone_HyperLink_Init.obj" \
"./isr/interrupt_vectors.obj" \
"./isr/interrupt.obj" \
"./msu/common/dsp0_imagefunc.obj" \
"./msu/common/dsp0_image_pre.obj" \
"./msu/common/dsp0_image.obj" \
"./msu/dsp0_core1.obj" \
"./msu/dsp0_core0.obj" \
"./navigator/Navigator_init_drv.obj" \
"./peripheral/peripheral.obj" \
"./peripheral/UART_Init_drv.obj" \
"./peripheral/EMIF16_Init.obj" \
"./serdes/Serdes_init.obj" \
"./src/dsp0_test.obj" \
"./src/dsp0_msu.obj" \
"./src/dsp0_mss.obj" \
"./src/dsp0_main.obj" \
"./src/dsp0_common.obj" \
"./srio/SRIO_Init_drv.obj" \
"./srio/SRIO_Init.obj" \
"../319.cmd" \
-l"libc.a" \

-include ../makefile.init

RM := DEL /F
RMDIR := RMDIR /S/Q

# All of the sources participating in the build are defined here
-include sources.mk
-include subdir_vars.mk
-include srio/subdir_vars.mk
-include src/subdir_vars.mk
-include serdes/subdir_vars.mk
-include peripheral/subdir_vars.mk
-include navigator/subdir_vars.mk
-include msu/subdir_vars.mk
-include msu/common/subdir_vars.mk
-include isr/subdir_vars.mk
-include hyperlink/subdir_vars.mk
-include edma/subdir_vars.mk
-include ddr/subdir_vars.mk
-include subdir_rules.mk
-include srio/subdir_rules.mk
-include src/subdir_rules.mk
-include serdes/subdir_rules.mk
-include peripheral/subdir_rules.mk
-include navigator/subdir_rules.mk
-include msu/subdir_rules.mk
-include msu/common/subdir_rules.mk
-include isr/subdir_rules.mk
-include hyperlink/subdir_rules.mk
-include edma/subdir_rules.mk
-include ddr/subdir_rules.mk
-include objects.mk

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(S_DEPS)),)
-include $(S_DEPS)
endif
ifneq ($(strip $(S_UPPER_DEPS)),)
-include $(S_UPPER_DEPS)
endif
ifneq ($(strip $(S62_DEPS)),)
-include $(S62_DEPS)
endif
ifneq ($(strip $(C64_DEPS)),)
-include $(C64_DEPS)
endif
ifneq ($(strip $(ASM_DEPS)),)
-include $(ASM_DEPS)
endif
ifneq ($(strip $(CC_DEPS)),)
-include $(CC_DEPS)
endif
ifneq ($(strip $(S55_DEPS)),)
-include $(S55_DEPS)
endif
ifneq ($(strip $(C67_DEPS)),)
-include $(C67_DEPS)
endif
ifneq ($(strip $(C??_DEPS)),)
-include $(C??_DEPS)
endif
ifneq ($(strip $(CLA_DEPS)),)
-include $(CLA_DEPS)
endif
ifneq ($(strip $(CPP_DEPS)),)
-include $(CPP_DEPS)
endif
ifneq ($(strip $(S??_DEPS)),)
-include $(S??_DEPS)
endif
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
ifneq ($(strip $(C62_DEPS)),)
-include $(C62_DEPS)
endif
ifneq ($(strip $(CXX_DEPS)),)
-include $(CXX_DEPS)
endif
ifneq ($(strip $(C++_DEPS)),)
-include $(C++_DEPS)
endif
ifneq ($(strip $(ASM_UPPER_DEPS)),)
-include $(ASM_UPPER_DEPS)
endif
ifneq ($(strip $(K_DEPS)),)
-include $(K_DEPS)
endif
ifneq ($(strip $(C43_DEPS)),)
-include $(C43_DEPS)
endif
ifneq ($(strip $(S67_DEPS)),)
-include $(S67_DEPS)
endif
ifneq ($(strip $(SA_DEPS)),)
-include $(SA_DEPS)
endif
ifneq ($(strip $(S43_DEPS)),)
-include $(S43_DEPS)
endif
ifneq ($(strip $(OPT_DEPS)),)
-include $(OPT_DEPS)
endif
ifneq ($(strip $(S64_DEPS)),)
-include $(S64_DEPS)
endif
ifneq ($(strip $(C_UPPER_DEPS)),)
-include $(C_UPPER_DEPS)
endif
ifneq ($(strip $(C55_DEPS)),)
-include $(C55_DEPS)
endif
endif

-include ../makefile.defs

# Add inputs and outputs from these tool invocations to the build variables 

# All Target
all: 319v1.out

# Tool invocations
319v1.out: $(OBJS) $(CMD_SRCS) $(GEN_CMDS)
	@echo 'Building target: $@'
	@echo 'Invoking: C6000 Linker'
	"D:/ti/ccsv5/tools/compiler/c6000_7.4.4/bin/cl6x" -mv6600 --abi=eabi -O2 --display_error_number --diag_warning=225 --diag_wrap=off -z -m"319v1.map" -i"D:/ti/ccsv5/tools/compiler/c6000_7.4.4/lib" -i"D:/ti/ccsv5/tools/compiler/c6000_7.4.4/include" --reread_libs --warn_sections --display_error_number --diag_wrap=off --xml_link_info="319v1_linkInfo.xml" --rom_model -o "319v1.out" $(ORDERED_OBJS)
	@echo 'Finished building target: $@'
	@echo ' '

# Other Targets
clean:
	-$(RM) $(C6000_EXECUTABLE_OUTPUTS__QUOTED) "319v1.out"
	-$(RM) "srio\SRIO_Init.pp" "srio\SRIO_Init_drv.pp" "src\dsp0_common.pp" "src\dsp0_main.pp" "src\dsp0_mss.pp" "src\dsp0_msu.pp" "src\dsp0_test.pp" "serdes\Serdes_init.pp" "peripheral\EMIF16_Init.pp" "peripheral\UART_Init_drv.pp" "peripheral\peripheral.pp" "navigator\Navigator_init_drv.pp" "msu\dsp0_core0.pp" "msu\dsp0_core1.pp" "msu\common\dsp0_image.pp" "msu\common\dsp0_image_pre.pp" "msu\common\dsp0_imagefunc.pp" "isr\interrupt.pp" "hyperlink\KeyStone_HyperLink_Init.pp" "edma\memory_transfer.pp" "ddr\DDR_Init.pp" 
	-$(RM) "srio\SRIO_Init.obj" "srio\SRIO_Init_drv.obj" "src\dsp0_common.obj" "src\dsp0_main.obj" "src\dsp0_mss.obj" "src\dsp0_msu.obj" "src\dsp0_test.obj" "serdes\Serdes_init.obj" "peripheral\EMIF16_Init.obj" "peripheral\UART_Init_drv.obj" "peripheral\peripheral.obj" "navigator\Navigator_init_drv.obj" "msu\dsp0_core0.obj" "msu\dsp0_core1.obj" "msu\common\dsp0_image.obj" "msu\common\dsp0_image_pre.obj" "msu\common\dsp0_imagefunc.obj" "isr\interrupt.obj" "isr\interrupt_vectors.obj" "hyperlink\KeyStone_HyperLink_Init.obj" "edma\memory_transfer.obj" "ddr\DDR_Init.obj" 
	-$(RM) "isr\interrupt_vectors.pp" 
	-@echo 'Finished clean'
	-@echo ' '

.PHONY: all clean dependents
.SECONDARY:

-include ../makefile.targets


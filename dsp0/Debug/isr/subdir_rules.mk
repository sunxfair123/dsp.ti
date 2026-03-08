################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Each subdirectory must supply rules for building sources it contributes
isr/interrupt.obj: ../isr/interrupt.c $(GEN_OPTS) $(GEN_HDRS)
	@echo 'Building file: $<'
	@echo 'Invoking: C6000 Compiler'
	"D:/ti/ccsv5/tools/compiler/c6000_7.4.4/bin/cl6x" -mv6600 --abi=eabi -Ooff -g --include_path="D:/ti/worksplace/319v1/dsp0/msu/include" --include_path="D:/ti/ccsv5/tools/compiler/c6000_7.4.4/include" --include_path="D:/ti/worksplace/319v1/include" --include_path="D:/ti/pdk_C6678_1_1_2_6/packages/ti/csl" --include_path="D:/ti/pdk_C6678_1_1_2_6/packages" --include_path="D:/ti/pdk_C6678_1_1_2_6/packages/ti/platform" --display_error_number --diag_warning=225 --diag_wrap=off --preproc_with_compile --preproc_dependency="isr/interrupt.pp" --obj_directory="isr" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: $<'
	@echo ' '

isr/interrupt_vectors.obj: ../isr/interrupt_vectors.asm $(GEN_OPTS) $(GEN_HDRS)
	@echo 'Building file: $<'
	@echo 'Invoking: C6000 Compiler'
	"D:/ti/ccsv5/tools/compiler/c6000_7.4.4/bin/cl6x" -mv6600 --abi=eabi -Ooff -g --include_path="D:/ti/worksplace/319v1/dsp0/msu/include" --include_path="D:/ti/ccsv5/tools/compiler/c6000_7.4.4/include" --include_path="D:/ti/worksplace/319v1/include" --include_path="D:/ti/pdk_C6678_1_1_2_6/packages/ti/csl" --include_path="D:/ti/pdk_C6678_1_1_2_6/packages" --include_path="D:/ti/pdk_C6678_1_1_2_6/packages/ti/platform" --display_error_number --diag_warning=225 --diag_wrap=off --preproc_with_compile --preproc_dependency="isr/interrupt_vectors.pp" --obj_directory="isr" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: $<'
	@echo ' '



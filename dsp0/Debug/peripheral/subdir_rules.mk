################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Each subdirectory must supply rules for building sources it contributes
peripheral/EMIF16_Init.obj: ../peripheral/EMIF16_Init.c $(GEN_OPTS) $(GEN_HDRS)
	@echo 'Building file: $<'
	@echo 'Invoking: C6000 Compiler'
	"D:/ti/ccsv5/tools/compiler/c6000_7.4.4/bin/cl6x" -mv6600 --abi=eabi -Ooff -g --include_path="D:/ti/worksplace/319v1/dsp0/msu/include" --include_path="D:/ti/ccsv5/tools/compiler/c6000_7.4.4/include" --include_path="D:/ti/worksplace/319v1/include" --include_path="D:/ti/pdk_C6678_1_1_2_6/packages/ti/csl" --include_path="D:/ti/pdk_C6678_1_1_2_6/packages" --include_path="D:/ti/pdk_C6678_1_1_2_6/packages/ti/platform" --display_error_number --diag_warning=225 --diag_wrap=off --preproc_with_compile --preproc_dependency="peripheral/EMIF16_Init.pp" --obj_directory="peripheral" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: $<'
	@echo ' '

peripheral/UART_Init_drv.obj: ../peripheral/UART_Init_drv.c $(GEN_OPTS) $(GEN_HDRS)
	@echo 'Building file: $<'
	@echo 'Invoking: C6000 Compiler'
	"D:/ti/ccsv5/tools/compiler/c6000_7.4.4/bin/cl6x" -mv6600 --abi=eabi -Ooff -g --include_path="D:/ti/worksplace/319v1/dsp0/msu/include" --include_path="D:/ti/ccsv5/tools/compiler/c6000_7.4.4/include" --include_path="D:/ti/worksplace/319v1/include" --include_path="D:/ti/pdk_C6678_1_1_2_6/packages/ti/csl" --include_path="D:/ti/pdk_C6678_1_1_2_6/packages" --include_path="D:/ti/pdk_C6678_1_1_2_6/packages/ti/platform" --display_error_number --diag_warning=225 --diag_wrap=off --preproc_with_compile --preproc_dependency="peripheral/UART_Init_drv.pp" --obj_directory="peripheral" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: $<'
	@echo ' '

peripheral/peripheral.obj: ../peripheral/peripheral.c $(GEN_OPTS) $(GEN_HDRS)
	@echo 'Building file: $<'
	@echo 'Invoking: C6000 Compiler'
	"D:/ti/ccsv5/tools/compiler/c6000_7.4.4/bin/cl6x" -mv6600 --abi=eabi -Ooff -g --include_path="D:/ti/worksplace/319v1/dsp0/msu/include" --include_path="D:/ti/ccsv5/tools/compiler/c6000_7.4.4/include" --include_path="D:/ti/worksplace/319v1/include" --include_path="D:/ti/pdk_C6678_1_1_2_6/packages/ti/csl" --include_path="D:/ti/pdk_C6678_1_1_2_6/packages" --include_path="D:/ti/pdk_C6678_1_1_2_6/packages/ti/platform" --display_error_number --diag_warning=225 --diag_wrap=off --preproc_with_compile --preproc_dependency="peripheral/peripheral.pp" --obj_directory="peripheral" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: $<'
	@echo ' '



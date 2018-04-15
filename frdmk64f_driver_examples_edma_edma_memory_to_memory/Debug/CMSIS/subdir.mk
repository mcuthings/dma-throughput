################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../CMSIS/system_MK64F12.c 

OBJS += \
./CMSIS/system_MK64F12.o 

C_DEPS += \
./CMSIS/system_MK64F12.d 


# Each subdirectory must supply rules for building sources it contributes
CMSIS/%.o: ../CMSIS/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: MCU C Compiler'
	arm-none-eabi-gcc -std=gnu99 -DPRINTF_FLOAT_ENABLE=0 -DFRDM_K64F -DCR_INTEGER_PRINTF -DFREEDOM -DSDK_DEBUGCONSOLE=0 -D__MCUXPRESSO -D__USE_CMSIS -DDEBUG -DCPU_MK64FN1M0VLL12_cm4 -DCPU_MK64FN1M0VLL12 -D__REDLIB__ -I"/Users/Hiro-MBP/Freescale/workspaces/k64/frdmk64f_driver_examples_edma_edma_memory_to_memory/board" -I"/Users/Hiro-MBP/Freescale/workspaces/k64/frdmk64f_driver_examples_edma_edma_memory_to_memory/source" -I"/Users/Hiro-MBP/Freescale/workspaces/k64/frdmk64f_driver_examples_edma_edma_memory_to_memory" -I"/Users/Hiro-MBP/Freescale/workspaces/k64/frdmk64f_driver_examples_edma_edma_memory_to_memory/drivers" -I"/Users/Hiro-MBP/Freescale/workspaces/k64/frdmk64f_driver_examples_edma_edma_memory_to_memory/CMSIS" -I"/Users/Hiro-MBP/Freescale/workspaces/k64/frdmk64f_driver_examples_edma_edma_memory_to_memory/utilities" -I"/Users/Hiro-MBP/Freescale/workspaces/k64/frdmk64f_driver_examples_edma_edma_memory_to_memory/startup" -O0 -fno-common -g -Wall -c  -ffunction-sections  -fdata-sections  -ffreestanding  -fno-builtin -fsingle-precision-constant -mcpu=cortex-m4 -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -D__REDLIB__ -specs=redlib.specs -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.o)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '



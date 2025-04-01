################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (13.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
C:/Users/nimsgernd/OneDrive\ -\ Milwaukee\ School\ of\ Engineering/Documents/Formula\ Hybrid/mp6_hcb/FH_HybridControlBoard/Utilities/ResourcesManager/res_mgr.c 

OBJS += \
./Utilities/res_mgr.o 

C_DEPS += \
./Utilities/res_mgr.d 


# Each subdirectory must supply rules for building sources it contributes
Utilities/res_mgr.o: C:/Users/nimsgernd/OneDrive\ -\ Milwaukee\ School\ of\ Engineering/Documents/Formula\ Hybrid/mp6_hcb/FH_HybridControlBoard/Utilities/ResourcesManager/res_mgr.c Utilities/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DCORE_CM4 -DUSE_HAL_DRIVER -DSTM32H755xx -DSTM32_THREAD_SAFE_STRATEGY=2 -DUSE_PWR_DIRECT_SMPS_SUPPLY -c -I../Core/Inc -I../../Common/Inc -I../../Drivers/STM32H7xx_HAL_Driver/Inc -I../../Drivers/STM32H7xx_HAL_Driver/Inc/Legacy -I../../Utilities/ResourcesManager -I../../Drivers/CMSIS/Device/ST/STM32H7xx/Include -I../../Drivers/CMSIS/Include -I../Core/ThreadSafe -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"Utilities/res_mgr.d" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Utilities

clean-Utilities:
	-$(RM) ./Utilities/res_mgr.cyclo ./Utilities/res_mgr.d ./Utilities/res_mgr.o ./Utilities/res_mgr.su

.PHONY: clean-Utilities


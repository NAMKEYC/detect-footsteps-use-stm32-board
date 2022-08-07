################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (10.3-2021.10)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Drivers/LMS303_Driver/Src/lsm303.c 

OBJS += \
./Drivers/LMS303_Driver/Src/lsm303.o 

C_DEPS += \
./Drivers/LMS303_Driver/Src/lsm303.d 


# Each subdirectory must supply rules for building sources it contributes
Drivers/LMS303_Driver/Src/%.o Drivers/LMS303_Driver/Src/%.su: ../Drivers/LMS303_Driver/Src/%.c Drivers/LMS303_Driver/Src/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F411xE -c -I../Core/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Drivers-2f-LMS303_Driver-2f-Src

clean-Drivers-2f-LMS303_Driver-2f-Src:
	-$(RM) ./Drivers/LMS303_Driver/Src/lsm303.d ./Drivers/LMS303_Driver/Src/lsm303.o ./Drivers/LMS303_Driver/Src/lsm303.su

.PHONY: clean-Drivers-2f-LMS303_Driver-2f-Src


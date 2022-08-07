################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (10.3-2021.10)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Drivers/driver_oled/Src/fonts.c \
../Drivers/driver_oled/Src/ssd1306.c 

OBJS += \
./Drivers/driver_oled/Src/fonts.o \
./Drivers/driver_oled/Src/ssd1306.o 

C_DEPS += \
./Drivers/driver_oled/Src/fonts.d \
./Drivers/driver_oled/Src/ssd1306.d 


# Each subdirectory must supply rules for building sources it contributes
Drivers/driver_oled/Src/%.o Drivers/driver_oled/Src/%.su: ../Drivers/driver_oled/Src/%.c Drivers/driver_oled/Src/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F411xE -c -I../Core/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Drivers-2f-driver_oled-2f-Src

clean-Drivers-2f-driver_oled-2f-Src:
	-$(RM) ./Drivers/driver_oled/Src/fonts.d ./Drivers/driver_oled/Src/fonts.o ./Drivers/driver_oled/Src/fonts.su ./Drivers/driver_oled/Src/ssd1306.d ./Drivers/driver_oled/Src/ssd1306.o ./Drivers/driver_oled/Src/ssd1306.su

.PHONY: clean-Drivers-2f-driver_oled-2f-Src


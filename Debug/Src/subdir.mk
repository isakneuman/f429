################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (9-2020-q2-update)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Src/dac.c \
../Src/delay.c \
../Src/gpio_periph.c \
../Src/hc_sr04.c \
../Src/i2c_periph.c \
../Src/jhd162a.c \
../Src/main.c \
../Src/nandflash.c \
../Src/syscalls.c \
../Src/sysmem.c \
../Src/terminal.c 

OBJS += \
./Src/dac.o \
./Src/delay.o \
./Src/gpio_periph.o \
./Src/hc_sr04.o \
./Src/i2c_periph.o \
./Src/jhd162a.o \
./Src/main.o \
./Src/nandflash.o \
./Src/syscalls.o \
./Src/sysmem.o \
./Src/terminal.o 

C_DEPS += \
./Src/dac.d \
./Src/delay.d \
./Src/gpio_periph.d \
./Src/hc_sr04.d \
./Src/i2c_periph.d \
./Src/jhd162a.d \
./Src/main.d \
./Src/nandflash.d \
./Src/syscalls.d \
./Src/sysmem.d \
./Src/terminal.d 


# Each subdirectory must supply rules for building sources it contributes
Src/%.o: ../Src/%.c Src/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DSTM32 -DSTM32F4 -DSTM32F429IGTx -DSTM32F429xx -DSTM32F427_437xx -c -I../Inc -I"C:/Users/isakn/Desktop/New folder/stm32lib/CMSIS-429/Include" -I"Q:/stm32courselearn/14_SPI_master_f438/STM32F4xx_StdPeriph_Driver/inc" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"


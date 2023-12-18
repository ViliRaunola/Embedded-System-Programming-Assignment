################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
LD_SRCS += \
../src/lscript.ld 

C_SRCS += \
../src/configuration.c \
../src/main.c \
../src/modulating.c \
../src/modulationCalculationTask.c \
../src/modulationPrintTask.c \
../src/utilities.c 

OBJS += \
./src/configuration.o \
./src/main.o \
./src/modulating.o \
./src/modulationCalculationTask.o \
./src/modulationPrintTask.o \
./src/utilities.o 

C_DEPS += \
./src/configuration.d \
./src/main.d \
./src/modulating.d \
./src/modulationCalculationTask.d \
./src/modulationPrintTask.d \
./src/utilities.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: ARM v7 gcc compiler'
	arm-none-eabi-gcc -Wall -O0 -g3 -c -fmessage-length=0 -MT"$@" -mcpu=cortex-a9 -mfpu=vfpv3 -mfloat-abi=hard -I../../assignment_bsp/ps7_cortexa9_0/include -I"G:\Opiskelu\Yliopisto\5.vuosi\EmbeddedSystemsProgramming\assignment\zybo_platform" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '



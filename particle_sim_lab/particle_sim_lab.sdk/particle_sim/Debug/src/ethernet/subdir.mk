################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/ethernet/ethernet.c \
../src/ethernet/ethernet_platform.c 

OBJS += \
./src/ethernet/ethernet.o \
./src/ethernet/ethernet_platform.o 

C_DEPS += \
./src/ethernet/ethernet.d \
./src/ethernet/ethernet_platform.d 


# Each subdirectory must supply rules for building sources it contributes
src/ethernet/%.o: ../src/ethernet/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: ARM v7 gcc compiler'
	arm-none-eabi-gcc -Wall -O0 -g3 -c -fmessage-length=0 -MT"$@" -mcpu=cortex-a9 -mfpu=vfpv3 -mfloat-abi=hard -I../../particle_ethernet_bsp/ps7_cortexa9_0/include -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '



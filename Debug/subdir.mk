################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
O_SRCS += \
../evalSpline.o 

C_SRCS += \
../drawSplines.c \
../evalSpline.c \
../v3d.c 

OBJS += \
./drawSplines.o \
./evalSpline.o \
./v3d.o 

C_DEPS += \
./drawSplines.d \
./evalSpline.d \
./v3d.d 


# Each subdirectory must supply rules for building sources it contributes
%.o: ../%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o"$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '



################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/Ensc351Part3-test.cpp 

OBJS += \
./src/Ensc351Part3-test.o 

CPP_DEPS += \
./src/Ensc351Part3-test.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -std=c++1y -I"/mnt/hgfs/VMsf/XModemProjects/XmodemFileTransferProject/ENSC351Part3/Ensc351part3/Ensc351Part2SolnLib" -I"/mnt/hgfs/VMsf/Ensc351" -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '



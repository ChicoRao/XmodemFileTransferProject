################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/ReceiverSS.cpp \
../src/SenderSS.cpp 

OBJS += \
./src/ReceiverSS.o \
./src/SenderSS.o 

CPP_DEPS += \
./src/ReceiverSS.d \
./src/SenderSS.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -std=c++1y -I"/mnt/hgfs/VMsf/XModemProjects/XmodemFileTransferProject/ENSC351Part3/Ensc351part3/Ensc351" -I"/mnt/hgfs/VMsf/XModemProjects/XmodemFileTransferProject/ENSC351Part3/Ensc351part3/Ensc351Part2SolnLib" -O0 -g3 -Wall -c -fmessage-length=0 -Wno-unused-variable -Wno-unknown-pragmas -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '



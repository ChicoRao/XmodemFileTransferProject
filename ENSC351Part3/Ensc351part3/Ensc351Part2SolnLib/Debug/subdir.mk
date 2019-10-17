################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../Ensc351Part2.cpp \
../Medium.cpp \
../PeerX.cpp \
../ReceiverX.cpp \
../SenderX.cpp \
../main.cpp \
../myIO.cpp 

OBJS += \
./Ensc351Part2.o \
./Medium.o \
./PeerX.o \
./ReceiverX.o \
./SenderX.o \
./main.o \
./myIO.o 

CPP_DEPS += \
./Ensc351Part2.d \
./Medium.d \
./PeerX.d \
./ReceiverX.d \
./SenderX.d \
./main.d \
./myIO.d 


# Each subdirectory must supply rules for building sources it contributes
%.o: ../%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -std=c++1y -I"/mnt/hgfs/VMsf/XModemProjects/XmodemFileTransferProject/ENSC351Part3/Ensc351part3/Ensc351" -I"/mnt/hgfs/VMsf/XModemProjects/XmodemFileTransferProject/ENSC351Part3/Ensc351part3/Ensc351Part2CodedBySmartState/src" -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '



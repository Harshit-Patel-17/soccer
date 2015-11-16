################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/Ball.cpp \
../src/Game.cpp \
../src/Ground.cpp \
../src/MultiPlayer.cpp \
../src/Player.cpp \
../src/SceneManager.cpp \
../src/Soccer.cpp \
../src/Welcome.cpp \
../src/main.cpp \
../src/state.cpp 

OBJS += \
./src/Ball.o \
./src/Game.o \
./src/Ground.o \
./src/MultiPlayer.o \
./src/Player.o \
./src/SceneManager.o \
./src/Soccer.o \
./src/Welcome.o \
./src/main.o \
./src/state.o 

CPP_DEPS += \
./src/Ball.d \
./src/Game.d \
./src/Ground.d \
./src/MultiPlayer.d \
./src/Player.d \
./src/SceneManager.d \
./src/Soccer.d \
./src/Welcome.d \
./src/main.d \
./src/state.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -std=c++11 -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '



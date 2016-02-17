#=============================================================================#
# Arduino CMake file
#=============================================================================#
set(CMAKE_TOOLCHAIN_FILE ../cmake/ArduinoToolchain.cmake) # Arduino Toolchain

cmake_minimum_required(VERSION 2.8)
#====================================================================#
#  Setup Project                                                     #
#====================================================================#
project(servoSpiControl)

set(${CMAKE_PROJECT_NAME}_BOARD mega)

IF("${COMPILER_SYSTEM}" MATCHES "Linux")
    set(${CMAKE_PROJECT_NAME}_PORT /dev/ttyACM0)
elseif("${COMPILER_SYSTEM}" MATCHES "Win")
    set(${CMAKE_PROJECT_NAME}_PORT com7)
else()
    message("Unable to know on what system we're comiling!!! : ${COMPILER_SYSTEM}")
endif()

set(mega.build.mcu atmega2560)
set(mega.upload.speed 115200)
set(mega.upload.protocol wiring)
set(ARDUINO_CXX_FLAGS "${ARDUINO_CXX_FLAGS} -std=c++11")

generate_arduino_library(Servo_lib
        SRCS Servo/Servo.cpp
        HDRS Servo/Servo.h
        BOARD mega)

set(${CMAKE_PROJECT_NAME}_SKETCH servoSpiControl.ino)
set(${CMAKE_PROJECT_NAME}_HDRS servoSpiControl.h)
set(${CMAKE_PROJECT_NAME}_LIBS Servo_lib)

generate_arduino_firmware(${CMAKE_PROJECT_NAME})


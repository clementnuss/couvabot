#=============================================================================#
# Arduino CMake file
#=============================================================================#
set(CMAKE_TOOLCHAIN_FILE ../cmake/ArduinoToolchain.cmake) # Arduino Toolchain

cmake_minimum_required(VERSION 2.8)
#====================================================================#
#  Setup Project                                                     #
#====================================================================#
project(servoSpiControl)

set(${CMAKE_PROJECT_NAME}_SKETCH  servoSpiControl.ino)
set(${CMAKE_PROJECT_NAME}_BOARD mega)

set(mega.build.mcu atmega2560)
set(mega.upload.speed 115200)
set(mega.upload.protocol wiring)

print_programmer_list()
print_board_list()

IF("${COMPILER_SYSTEM}" MATCHES "Linux")
    set(${CMAKE_PROJECT_NAME}_PORT /dev/ttyACM0)
elseif("${COMPILER_SYSTEM}" MATCHES "Win")
    set(${CMAKE_PROJECT_NAME}_PORT com7)
else()
		message("Unable to know on what system we're comiling!!! : ${COMPILER_SYSTEM}")
endif()

generate_arduino_firmware(${CMAKE_PROJECT_NAME})


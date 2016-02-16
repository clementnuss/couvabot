#=============================================================================#
# Arduino CMake file
#=============================================================================#
set(CMAKE_TOOLCHAIN_FILE ../cmake/ArduinoToolchain.cmake) # Arduino Toolchain


cmake_minimum_required(VERSION 2.8)
#====================================================================#
#  Setup Project                                                     #
#====================================================================#
project(ArduinoSPI)

set(${CMAKE_PROJECT_NAME}_SKETCH  spiControl.ino)
set(${CMAKE_PROJECT_NAME}_BOARD mega)

IF("${COMPILER_SYSTEM}" MATCHES "Linux")
    message("At 1: asddasFOO is defined")
    set(${CMAKE_PROJECT_NAME}_PORT /dev/ttyACM0)
elseif("${COMPILER_SYSTEM}" MATCHES "Win")
    set(${CMAKE_PROJECT_NAME}_PORT com3)
else()
		message("Unable to know on what system we're comiling!!! : ${COMPILER_SYSTEM}")
endif()


generate_arduino_firmware(${CMAKE_PROJECT_NAME})

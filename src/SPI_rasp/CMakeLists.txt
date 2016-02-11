cmake_minimum_required(VERSION 3.3)
project(SPI_rasp)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

set(SOURCE_FILES main.cpp)
add_executable(SPI_rasp ${SOURCE_FILES})

include_directories(../RPi/bcm2835-1.49/src/)
link_directories(../RPi/bcm2835-1.49/src)
cmake_minimum_required(VERSION 3.0)
project(SPI_rasp)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

include_directories(../RPi/bcm2835-1.49/src/)
link_directories(../RPi/bcm2835-1.49/src)

set(SOURCE_FILES main.cpp)
add_executable(SPI_rasp ${SOURCE_FILES})
target_link_libraries( SPI_rasp bcm2835)

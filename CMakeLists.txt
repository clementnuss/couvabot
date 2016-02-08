cmake_minimum_required(VERSION 3.3)
project(git_couvabot)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

set(SOURCE_FILES
    src/RPi/bcm2835-1.49/examples/blink/blink.c
    src/RPi/bcm2835-1.49/examples/event/event.c
    src/RPi/bcm2835-1.49/examples/gpio/gpio.c
    src/RPi/bcm2835-1.49/examples/i2c/i2c.c
    src/RPi/bcm2835-1.49/examples/input/input.c
    src/RPi/bcm2835-1.49/examples/pwm/pwm.c
    src/RPi/bcm2835-1.49/examples/spi/spi.c
    src/RPi/bcm2835-1.49/examples/spin/spin.c
    src/RPi/bcm2835-1.49/src/bcm2835.c
    src/RPi/bcm2835-1.49/src/bcm2835.h
    src/RPi/bcm2835-1.49/src/test.c
    src/SPI/helloArduino.c
    src/SPI/spi.c)

include_directories(src/RPi/bcm2835-1.49/src/)
link_directories(src/RPi/bcm2835-1.49/src)

add_executable(git_couvabot ${SOURCE_FILES})
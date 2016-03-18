cmake_minimum_required(VERSION 3.0)
project(couvabot)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/bin")

if (UNIX)
    include_directories(${OpenCV_INCLUDE_DIRS})
    set(CMAKE_MODULE_PATH /usr/local/lib/cmake)
    find_package(OpenCV REQUIRED)
    find_package(raspicam REQUIRED)
elseif (WIN32)
    find_package(OpenCV REQUIRED)
    set(OpenCV_DIR "C:\\OpenCV\\opencv-2.4.11")
endif ()


include_directories(../RPi/bcm2835-1.49/src/)
link_directories(../RPi/bcm2835-1.49/src)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -fpermissive")

set(SOURCE_FILES
        src/robotConstants.h
        src/imageProcessing/camera.cpp
        src/imageProcessing/detectObjects.cpp
        src/imageProcessing/Blob.cpp
        src/navigation/Trajectory.cpp
        src/navigation/differentialDrive.cpp
        src/navigation/braitenberg.cpp
        src/physIface/localTime.cpp
        src/physIface/SPICom.cpp
        src/physIface/arduinoComm.cpp)

add_executable(main ${SOURCE_FILES} src/main.cpp)

if (UNIX)
    target_link_libraries(main ${OpenCV_LIBS} bcm2835 ${raspicam_CV_LIBS} pthread)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -mfloat-abi=hard -mfpu=vfp")
    #Amélioration des performances
    #https://www.raspberrypi.org/forums/viewtopic.php?f=63&t=115795

elseif (WIN32)
    target_link_libraries(main ${OpenCV_LIBS})
endif ()


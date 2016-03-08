cmake_minimum_required(VERSION 3.0)
project(couvabot)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/bin")

if(UNIX)
    include_directories(~/raspicam/cppapi/build/install/include/raspicam)
    include_directories(${OpenCV_INCLUDE_DIRS})

elseif(WIN32)
    set(OpenCV_DIR "C:\\OpenCV\\opencv-2.4.11")
endif()

find_package(OpenCV REQUIRED)

include_directories(../RPi/bcm2835-1.49/src/)
link_directories(../RPi/bcm2835-1.49/src)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

set(SOURCE_FILES src/camera.cpp src/detectObjects.cpp src/Blob.cpp src/SPICom.cpp src/mvmtController.cpp main.cpp)
add_executable(main ${SOURCE_FILES})

if(UNIX)
    target_link_libraries(main ${OpenCV_LIBS}  bcm2835)

elseif(WIN32)
    target_link_libraries(main ${OpenCV_LIBS})
endif()


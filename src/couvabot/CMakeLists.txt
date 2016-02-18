cmake_minimum_required(VERSION 3.2)
project(couvabot)
find_package(OpenCV REQUIRED)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/bin")

include_directories(${OpenCV_INCLUDE_DIRS})
include_directories(~/raspicam/cppapi/build/install/include/raspicam)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

set(SOURCE_FILES main.cpp)
add_executable(main ${SOURCE_FILES})
add_executable(objTrack src/objTrack.cpp src/objTrack.h src/Blob.cpp src/Blob.h)

target_link_libraries(objTrack ${OpenCV_LIBS})
target_link_libraries(main ${OpenCV_LIBS})


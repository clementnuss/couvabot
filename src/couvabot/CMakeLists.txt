cmake_minimum_required(VERSION 3.2)
project(couvabot)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/bin")

if(UNIX)
    include_directories(~/raspicam/cppapi/build/install/include/raspicam)
    include_directories(${OpenCV_INCLUDE_DIRS})

elseif(WIN32)
    set(OpenCV_DIR "C:\\OpenCV\\opencv-2.4.11")
endif()

find_package(OpenCV REQUIRED)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

set(SOURCE_FILES main.cpp src/camera.cpp src/detectObjects.cpp src/Blob.cpp)
add_executable(main ${SOURCE_FILES})

target_link_libraries(main ${OpenCV_LIBS})


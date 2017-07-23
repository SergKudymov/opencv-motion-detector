cmake_minimum_required(VERSION 2.8)

project(PlayVideo)

find_package( OpenCV REQUIRED )

set(CMAKE_CXX_STANDARD 11)

set(SOURCE_FILES main.cpp)
add_executable(PlayVideo ${SOURCE_FILES})

target_link_libraries( PlayVideo ${OpenCV_LIBS} )

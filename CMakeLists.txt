cmake_minimum_required(VERSION 3.22)
project(main.cpp)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_WIN32_EXECUTABLE ON)

file(GLOB_RECURSE COMMON_FILES src/*.cpp include/*.h)

add_executable(main main.cpp ${COMMON_FILES})
target_include_directories(main PRIVATE include/)
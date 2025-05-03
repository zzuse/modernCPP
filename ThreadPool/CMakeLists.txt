cmake_minimum_required(VERSION 3.25)
project(ThreadPool)

message("CC version = ${CMAKE_CXX_COMPILER_VERSION}")

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_BUILD_TYPE Debug)

add_executable(ThreadPool main.cpp)

# default install gcc13
# export CC=/opt/local/bin/gcc-mp-13
# export CXX=/opt/local/bin/g++-mp-13
# cmake .. or /opt/local/bin/g++-mp-13 -std=c++20 -o main main.cpp
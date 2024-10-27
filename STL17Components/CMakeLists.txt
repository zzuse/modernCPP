cmake_minimum_required(VERSION 3.25)
project(STL17Components)

message("CC version = ${CMAKE_CXX_COMPILER_VERSION}")

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_BUILD_TYPE Debug)

add_executable(STL17Components main.cpp)
include(FetchContent)
FetchContent_Declare(
        poolSTL
        GIT_REPOSITORY https://github.com/alugowski/poolSTL
        GIT_TAG main
        GIT_SHALLOW TRUE
)
FetchContent_MakeAvailable(poolSTL)

target_link_libraries(STL17Components poolSTL::poolSTL)

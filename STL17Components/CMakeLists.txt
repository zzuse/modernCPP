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

# alternative 1
# https://github.com/alugowski/poolSTL
# mkdir build && cd build && cmake ..
# cmake --build .  -v

# alternative 2
# git clone onetbb, make and install tbb
# cmake -DCMAKE_INSTALL_PREFIX=/tmp/my_installed_onetbb -DTBB_TEST=OFF ..
# clang++ main.cpp -std=c++20 -o myprogram -I/opt/intel/tbb/include -L/opt/intel/tbb/lib -ltbb
# export LD_LIBRARY_PATH=/opt/intel/tbb/lib:$LD_LIBRARY_PATH

# default install gcc13
# export CC=/opt/local/bin/gcc-mp-13
# export CXX=/opt/local/bin/g++-mp-13
# cmake .. or /opt/local/bin/g++-mp-13 -std=c++20 -o main main.cpp
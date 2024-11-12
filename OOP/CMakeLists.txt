cmake_minimum_required(VERSION 3.25)
project(OOP)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_BUILD_TYPE Debug)
add_executable(OOP main.cpp Account.cpp Savings.cpp Checking.cpp Transaction.cpp)

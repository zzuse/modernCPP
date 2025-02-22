// INSTALL: sudo port install gcc13
// COMPILE: /opt/local/bin/g++-mp-13 -std=c++20 -fcoroutines -pthread -o main main.cpp
// jthread only supported by gcc
#include <atomic>
#include <iostream>
#include <thread>
#include <vector>

void run_code()
{
    std::atomic_flag flag1 = true;
    std::atomic_flag flag2 = ATOMIC_FLAG_INIT;
    std::cout << "1. previous flag value: " << flag2.test_and_set() << std::endl;
    std::cout << "2. previous flag value: " << flag2.test_and_set() << std::endl;
    flag2.clear();
    std::cout << "3. previous flag value: " << flag2.test_and_set() << std::endl;
}

int main()
{
    run_code();
    return 0;
}
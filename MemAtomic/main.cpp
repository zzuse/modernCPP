// INSTALL: sudo port install gcc13
// COMPILE: /opt/local/bin/g++-mp-13 -std=c++20 -fcoroutines -pthread -o main main.cpp
// jthread only supported by gcc
#include <atomic>
#include <iostream>
#include <thread>
#include <vector>

void run_flag()
{
    std::atomic_flag flag1 = true;
    std::atomic_flag flag2 = ATOMIC_FLAG_INIT;
    std::cout << "1. previous flag value: " << flag2.test_and_set() << std::endl;
    std::cout << "2. previous flag value: " << flag2.test_and_set() << std::endl;
    flag2.clear();
    std::cout << "3. previous flag value: " << flag2.test_and_set() << std::endl;
}

void run_bool()
{
    std::atomic<bool> flag_1;
    std::cout << "flag 1 = " << flag_1 << std::endl;

    // not compile because atomic_bool is not copyable and not assignable
    // std::atomic<bool> flag_2(flag_1);
    // std::atomic<bool> flag_3 = flag_1;

    bool non_atomic_bool = true;
    std::atomic<bool> flag_4(non_atomic_bool);
    std::cout << "flag 4 = " << flag_4 << std::endl;
    std::atomic<bool> flag_5 = non_atomic_bool;
    std::cout << "flag 5 = " << flag_5 << std::endl;
}

int main()
{
    run_flag();
    run_bool();
    return 0;
}
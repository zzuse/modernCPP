#include "common.h"

#include <iostream>
#include <list>
#include <mutex>
#include <thread>

/*
 * lock - locks the mutex, blocks if the mutex is not available
 * try_lock - tries to lock the mutex, returns if the mutex is not available
 * unlock - unlocks the mutex
 */
std::list<int> my_list;
std::mutex m;

void add_to_list(int const &x)
{
    // calling lock and unlock is a burden, so we change to lock_guard
    std::lock_guard<std::mutex> lg(m);
    // m.lock();
    my_list.push_front(x);
    // m.unlock();
}

void size()
{
    std::lock_guard<std::mutex> lg(m);
    // m.lock();
    int size = my_list.size();
    // m.unlock();
    std::cout << "size of the list is : " << size << std::endl;
}

void run_code()
{
    std::thread thread_1(add_to_list, 4);
    std::thread thread_2(add_to_list, 11);
    thread_1.join();
    thread_2.join();
    for (auto x : my_list) {
        std::cout << x << std::endl;
    }
}

int main()
{
    run_code();
    return 0;
}
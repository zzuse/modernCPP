// INSTALL: sudo port install gcc13
// COMPILE: /opt/local/bin/g++-mp-13 -std=c++20 -fcoroutines -pthread -o main main.cpp
// jthread only supported by gcc
#include "common.h"
#include <atomic>
#include <chrono>
#include <iostream>

class thread_pool {
    std::atomic_bool done;
    threadsafe_queue<std::function<void()>> work_queue;

    std::vector<std::thread> threads;
    join_threads joiner;

    void worker_thread()
    {
        while (!done) {
            std::function<void()> task;
            if (work_queue.try_pop(task)) {
                task();
            } else {
                std::this_thread::yield();
            }
        }
    }

public:
    thread_pool()
        : done(false)
        , joiner(threads)
    {
        int const thread_count = std::thread::hardware_concurrency();
        try {
            for (int i = 0; i < thread_count; i++) {
                threads.push_back(std::thread(&thread_pool::worker_thread, this));
            }
        } catch (...) {
            done = true;
            throw;
        }
    }
    ~thread_pool() { done = true; }

    template <typename Function_type>
    void submit(Function_type f)
    {
        work_queue.push(std::function<void()>(f));
    }
};

void run()
{
    thread_pool pool;
    std::cout << "Testing thread pool" << std::endl;
    for (int i = 0; i < 100; i++) {
        pool.submit([=] { printf(" %d printed by thread = %d \n", i, std::this_thread::get_id()); });
    }

    getchar();
}

int main()
{
    run();
    return 0;
}
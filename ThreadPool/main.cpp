// INSTALL: sudo port install gcc13
// COMPILE: /opt/local/bin/g++-mp-13 -std=c++20 -fcoroutines -pthread -o main main.cpp
// jthread only supported by gcc
#include "common.h"
#include <atomic>
#include <chrono>
#include <functional>
#include <future>
#include <iostream>
#include <numeric>

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

class thread_pool_waiting {
    std::atomic_bool done;
    threadsafe_queue<function_wrapper> work_queue;

    std::vector<std::thread> threads;
    join_threads joiner;

    void worker_thread()
    {
        while (!done) {
            function_wrapper task;
            if (work_queue.try_pop(task)) {
                task();
            } else {
                std::this_thread::yield();
            }
        }
    }

public:
    thread_pool_waiting()
        : done(false)
        , joiner(threads)
    {
        int const thread_count = std::thread::hardware_concurrency();
        try {
            for (int i = 0; i < thread_count; i++) {
                threads.push_back(std::thread(&thread_pool_waiting::worker_thread, this));
            }
        } catch (...) {
            done = true;
            throw;
        }
    }
    ~thread_pool_waiting() { done = true; }
    /*
    Why using function_wrapper, because it is movable
    std::packaged_task is a callable wrapper that encapsulates a function or callable object and allows its result to be
    retrieved asynchronously via a std::future. It is not copyable but is movable. This means you cannot have multiple
    std::packaged_task instances sharing ownership of the same task. Once a std::packaged_task is moved, the original
    instance becomes invalid.
    std::shared_ptr is a smart pointer that provides shared ownership of a dynamically allocated object. Multiple
    std::shared_ptr instances can share ownership of the same object, and the object is destroyed only when the last
    std::shared_ptr is destroyed or reset. It is both copyable and movable, making it suitable for scenarios where
    multiple entities need to share ownership of the same resource.
    */
    template <typename Function_type>
    std::future<typename std::result_of<Function_type()>::type> submit(Function_type f)
    {
        typedef typename std::result_of<Function_type()>::type result_type;
        std::packaged_task<result_type()> task(std::move(f));
        std::future<result_type> res(task.get_future());
        work_queue.push(std::move(task));
        return res;
    }
};

template <typename Iterator, typename T>
struct accumulate_block {
    T operator()(Iterator first, Iterator last)
    {
        T value = std::accumulate(first, last, T());
        printf(" %d - %d  \n", std::this_thread::get_id(), value);
        return value;
    }
};

template <typename Iterator, typename T>
T parallel_accumulate(Iterator first, Iterator last, T init)
{
    unsigned long const length = std::distance(first, last);
    thread_pool_waiting pool;

    if (!length) return init;

    unsigned long const min_per_thread = 25;
    unsigned long const max_threads = (length + min_per_thread - 1) / min_per_thread;

    unsigned long const hardware_threads = std::thread::hardware_concurrency();

    unsigned long const num_threads = std::min(hardware_threads != 0 ? hardware_threads : 2, max_threads);

    unsigned long const block_size = length / num_threads;

    std::vector<std::future<T>> futures(num_threads - 1);

    Iterator block_start = first;
    for (unsigned long i = 0; i < (num_threads - 1); ++i) {
        Iterator block_end = block_start;
        std::advance(block_end, block_size);
        futures[i] = pool.submit(std::bind(accumulate_block<Iterator, T>(), block_start, block_end));
        block_start = block_end;
    }
    T last_result = accumulate_block<int*, int>()(block_start, last);

    T result = init;
    for (unsigned long i = 0; i < (num_threads - 1); ++i) {
        result += futures[i].get();
    }
    result += last_result;
    return result;
}

void run_thread_pool_wait()
{
    const int size = 1000;
    int* my_array = new int[size];
    srand(0);
    for (size_t i = 0; i < size; i++) {
        my_array[i] = 1;
    }
    long result = parallel_accumulate<int*, int>(my_array, my_array + size, 0);
    std::cout << "final sum is - " << result << std::endl;
}

int main()
{
    run();
    run_thread_pool_wait();
    return 0;
}
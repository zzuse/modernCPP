// INSTALL: sudo port install gcc13
// COMPILE: /opt/local/bin/g++-mp-13 -std=c++20 -fcoroutines -pthread -o main main.cpp
// jthread only supported by gcc
#include "common.h"
#include <atomic>
#include <chrono>
#include <functional>
#include <future>
#include <iostream>
#include <list>
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

// computes the sum of the elements in the range
template <typename Iterator, typename T>
struct accumulate_block {
    T operator()(Iterator first, Iterator last)
    {
        // computes the sum of the elements in the specified range, starting with an initial value of T().
        T value = std::accumulate(first, last, T());
        printf(" %d - %d  \n", std::this_thread::get_id(), value);
        return value;
    }
};

// It divides the input range into smaller blocks, processes each block in a separate thread, and combines the results.
template <typename Iterator, typename T>
T parallel_accumulate(Iterator first, Iterator last, T init)
{
    unsigned long const length = std::distance(first, last);
    thread_pool_waiting pool;

    // Handle Empty Range
    if (!length) return init;

    unsigned long const min_per_thread = 25;
    unsigned long const max_threads = (length + min_per_thread - 1) / min_per_thread;

    unsigned long const hardware_threads = std::thread::hardware_concurrency();

    unsigned long const num_threads = std::min(hardware_threads != 0 ? hardware_threads : 2, max_threads);

    unsigned long const block_size = length / num_threads;

    // The input range is divided into num_threads blocks
    // task is created using std::bind to bind the accumulate_block functor to the block's range.
    std::vector<std::future<T>> futures(num_threads - 1);
    Iterator block_start = first;
    for (unsigned long i = 0; i < (num_threads - 1); ++i) {
        Iterator block_end = block_start;
        std::advance(block_end, block_size);
        futures[i] = pool.submit(std::bind(accumulate_block<Iterator, T>(), block_start, block_end));
        block_start = block_end;
    }
    // The last block is processed in the current thread
    T last_result = accumulate_block<int*, int>()(block_start, last);
    // Combine Results
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

class thread_pool_waiting_other_tasks {
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
    thread_pool_waiting_other_tasks()
        : done(false)
        , joiner(threads)
    {
        int const thread_count = std::thread::hardware_concurrency();
        try {
            for (int i = 0; i < thread_count; i++) {
                threads.push_back(std::thread(&thread_pool_waiting_other_tasks::worker_thread, this));
            }
        } catch (...) {
            done = true;
            throw;
        }
    }
    ~thread_pool_waiting_other_tasks() { done = true; }

    template <typename Function_type>
    std::future<typename std::result_of<Function_type()>::type> submit(Function_type f)
    {
        typedef typename std::result_of<Function_type()>::type result_type;
        std::packaged_task<result_type()> task(std::move(f));
        std::future<result_type> res(task.get_future());
        work_queue.push(std::move(task));
        return res;
    }

    // executed by invoking it, if no tasks are available, the function may Yield
    void run_pending_task()
    {
        function_wrapper task;
        if (work_queue.try_pop(task)) {
            task();
        } else {
            std::this_thread::yield();
        }
    }
};

// parallel sorting of a std::list using a thread pool
template <typename T>
struct sorter {
    thread_pool_waiting_other_tasks pool;

    std::list<T> do_sort(std::list<T>& chunk_data)
    {
        if (chunk_data.size() < 2) return chunk_data;
        std::list<T> result;
        // The splice method transfers elements between lists by modifying the internal pointers of the lists.
        // transfers the first element of chunk_data to the beginning of the result list
        result.splice(result.begin(), chunk_data, chunk_data.begin());

        // The list is divided into two parts: elements less than the partition value and elements greater than or equal
        // to it. This is done using std::partition
        T const& partition_val = *result.begin();
        typename std::list<T>::iterator divide_point
            = std::partition(chunk_data.begin(), chunk_data.end(), [&](T const& val) { return val < partition_val; });

        // A new list (new_lower_chunk) is created to hold the elements less than the partition value, and submitted to
        // the thread pool to sort this lower partition asynchronously
        std::list<T> new_lower_chunk;
        // transfers a range of elements (those less than the partition value) from chunk_data to new_lower_chunk
        new_lower_chunk.splice(new_lower_chunk.end(), chunk_data, chunk_data.begin(), divide_point);
        std::future<std::list<T>> new_lower
            = pool.submit(std::bind(&sorter::do_sort, this, std::move(new_lower_chunk)));

        // The remaining elements (greater than or equal to the partition value) are sorted recursively in the current
        // thread. The sorted higher partition is appended to the result list.
        std::list<T> new_higher(do_sort(chunk_data));
        result.splice(result.end(), new_higher);
        // while (!new_lower._Is_ready()) {
        while (new_lower.wait_for(std::chrono::seconds(0)) == std::future_status::timeout) {
            // While waiting for the lower partition to finish sorting, the run_pending_task method of the thread pool
            // is called to execute other pending tasks, ensuring efficient use of CPU resources.
            pool.run_pending_task();
        }
        // using new_lower.get() merged into the final result list.
        result.splice(result.begin(), new_lower.get());
        return result;
    }
};

template <typename T>
std::list<T> parallel_quick_sort(std::list<T> input)
{
    if (input.empty()) {
        return input;
    }
    sorter<T> s;
    return s.do_sort(input);
}

void run_thread_pool_wait_other_task()
{
    const int size = 800;
    std::list<int> my_array;

    srand(0);

    for (size_t i = 0; i < size; i++) {
        my_array.push_back(rand());
    }
    my_array = parallel_quick_sort(my_array);

    for (size_t i = 0; i < size; i++) {
        std::cout << my_array.front() << std::endl;
        my_array.pop_front();
    }
}

int main()
{
    run();
    run_thread_pool_wait();
    run_thread_pool_wait_other_task();
    return 0;
}
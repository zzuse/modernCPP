// INSTALL: sudo port install gcc13
// COMPILE: /opt/local/bin/g++-mp-13 -std=c++20 -o main main.cpp
// There are three alternatives for parallel stls execution policy:
// 1. Using poolstl/poolstl.hpp, std::sort(poolstl::par, dataset.begin(), dataset.end());
// 2. Using tbb::parallel_sort(dataset.begin(), dataset.end());
// 3. Using std::sort(std::execution::par, dataset.begin(), dataset.end()); but need gcc-13 support
// Other method not working: tried clang++-mp-17, not support execution policy.
// Another method not working: used tbb as stl backend, compile error, not easy to work with c++20. I don't have
// ablility to dig any more.
//
// #include <poolstl/poolstl.hpp>
// #include "tbb/parallel_sort.h"
// #include "tbb/tbb.h"
#include <algorithm>
#include <chrono>
#include <execution>
#include <future>
#include <iostream>
#include <list>
#include <random>
#include <ratio>
#include <thread>
#include <vector>

const size_t testSize = 1000000;
const int iterationCount = 5;

void print_results(const char* const tag, std::chrono::high_resolution_clock::time_point startTime,
                   std::chrono::high_resolution_clock::time_point endTime)
{
    printf("%s: Time: %fms\n", tag,
           std::chrono::duration_cast<std::chrono::duration<double, std::milli>>(endTime - startTime).count());
}

class join_threads {
    std::vector<std::thread>& threads;

public:
    explicit join_threads(std::vector<std::thread>& _threads)
        : threads(_threads)
    {
    }
    ~join_threads()
    {
        for (long i = 0; i < threads.size(); i++) {
            if (threads[i].joinable()) threads[i].join();
        }
    }
};

void one_million_sort()
{
    std::random_device rd;
    printf("Testing with %zu doubles...\n", testSize);
    std::vector<double> doubles(testSize);
    for (auto& d : doubles) {
        d = static_cast<double>(rd());
    }
    for (int i = 0; i < iterationCount; ++i) {
        std::vector<double> sorted(doubles);
        const auto startTime = std::chrono::high_resolution_clock::now();
        sort(sorted.begin(), sorted.end());
        const auto endTime = std::chrono::high_resolution_clock::now();
        std::cout << "Begin: " << sorted.front() << " End: " << sorted.back() << " ";
        print_results("Serial STL", startTime, endTime);
    }
    for (int i = 0; i < iterationCount; ++i) {
        std::vector<double> sorted(doubles);
        const auto startTime = std::chrono::high_resolution_clock::now();
        std::sort(std::execution::par, sorted.begin(), sorted.end());
        const auto endTime = std::chrono::high_resolution_clock::now();
        std::cout << "Begin: " << sorted.front() << " End: " << sorted.back() << " ";
        print_results("Parallel STL", startTime, endTime);
    }
}

template <typename T>
std::list<T> sequential_quick_sort(std::list<T> input)
{
    if (input.size() < 2) {
        return input;
    }
    std::list<T> result;
    result.splice(result.begin(), input, input.begin());
    T pivot = *result.begin();

    // arrange the input array
    auto divide_point = std::partition(input.begin(), input.end(), [&](T const& t) { return t < pivot; });
    std::list<T> lower_list;
    lower_list.splice(lower_list.end(), input, input.begin(), divide_point);

    auto new_lower(sequential_quick_sort(std::move(lower_list)));
    auto new_upper(sequential_quick_sort(std::move(input)));
    // arranging the result list
    result.splice(result.begin(), new_lower);
    result.splice(result.end(), new_upper);
    return result;
}

template <typename T>
std::list<T> parallel_quick_sort(std::list<T> input)
{
    if (input.size() < 2) {
        return input;
    }
    std::list<T> result;
    result.splice(result.begin(), input, input.begin());
    T pivot = *result.begin();

    // arrange the input array
    auto divide_point = std::partition(input.begin(), input.end(), [&](T const& t) { return t < pivot; });
    std::list<T> lower_list;
    lower_list.splice(lower_list.end(), input, input.begin(), divide_point);

    auto new_lower(parallel_quick_sort(std::move(lower_list)));
    std::future<std::list<T>> new_upper_future(std::async(&parallel_quick_sort<T>, std::move(input)));
    // arranging the result list
    result.splice(result.begin(), new_lower);
    result.splice(result.end(), new_upper_future.get());
    return result;
}

void one_million_quick_sort()
{
    std::random_device rd;
    printf("Testing with %zu doubles...\n", testSize);
    std::list<double> doubles(testSize);
    for (auto& d : doubles) {
        d = static_cast<double>(rd());
    }
    for (int i = 0; i < iterationCount; ++i) {
        std::list<double> sorted(doubles);
        const auto startTime = std::chrono::high_resolution_clock::now();
        sequential_quick_sort(sorted);
        const auto endTime = std::chrono::high_resolution_clock::now();
        // printf(": Lowest: %g Highest: %g Time: %fms\n", sorted.begin(), sorted.end(),
        //        std::chrono::duration_cast<std::chrono::duration<double, std::milli>>(endTime - startTime).count());
        std::cout << "Begin: " << sorted.front() << " End: " << sorted.back() << " ";
        print_results("Serial quick sort", startTime, endTime);
    }
    for (int i = 0; i < iterationCount; ++i) {
        std::list<double> sorted(doubles);
        const auto startTime = std::chrono::high_resolution_clock::now();
        parallel_quick_sort(sorted);
        const auto endTime = std::chrono::high_resolution_clock::now();
        // printf("Parallel quick sort: Lowest: %g Highest: %g Time: %fms\n", sorted.begin(), sorted.end(),
        //    std::chrono::duration_cast<std::chrono::duration<double, std::milli>>(endTime - startTime).count());
        std::cout << "Begin: " << sorted.front() << " End: " << sorted.back() << " ";
        print_results("Parallel quick sort", startTime, endTime);
    }
}

template <typename Iterator, typename Func>
void parallel_for_each_pt(Iterator first, Iterator last, Func f)
{
    unsigned long const length = std::distance(first, last);
    if (!length) return;
    unsigned long const min_per_thread = 25;
    unsigned long const max_threads = (length + min_per_thread - 1) / min_per_thread;

    unsigned long const hardware_threads = std::thread::hardware_concurrency();
    unsigned long const num_threads = std::min(hardware_threads != 0 ? hardware_threads : 2, max_threads);
    unsigned long const block_size = length / num_threads;

    std::vector<std::future<void>> futures(num_threads - 1);
    std::vector<std::thread> threads(num_threads - 1);
    join_threads joiner(threads);

    Iterator block_start = first;
    for (unsigned long i = 0; i <= (num_threads - 2); i++) {
        Iterator block_end = block_start;
        std::advance(block_end, block_size);

        std::packaged_task<void(void)> task([=]() { std::for_each(block_start, block_end, f); });
        futures[i] = task.get_future();
        threads[i] = std::thread(std::move(task));
        block_start = block_end;
    }

    std::for_each(block_start, last, f);
    for (unsigned long i = 0; i < (num_threads - 1); ++i) futures[i].get();
}

template <typename Iterator, typename Func>
void parallel_for_each_async(Iterator first, Iterator last, Func f)
{
    unsigned long const length = std::distance(first, last);
    if (!length) return;
    unsigned long const min_per_thread = 25;
    if (length < 2 * min_per_thread) {
        std::for_each(first, last, f);
    } else {
        Iterator const mid_point = first + length / 2;
        std::future<void> first_half = std::async(&parallel_for_each_async<Iterator, Func>, first, mid_point, f);
        parallel_for_each_async(mid_point, last, f);
        first_half.get();
    }
}

void one_million_parallel_for()
{
    std::vector<int> ints(1000);
    for (auto& i : ints) {
        i = 1;
    }
    auto long_function = [](const int& n) {
        int sum = 0;
        for (auto i = 0; i < 100000; i++) {
            sum += 1 * (i - 499);
        }
    };
    auto startTime = std::chrono::high_resolution_clock::now();
    std::for_each(ints.cbegin(), ints.cend(), long_function);
    auto endTime = std::chrono::high_resolution_clock::now();
    std::cout << "Begin: " << ints.front() << " End: " << ints.back() << " ";
    print_results("std::for_each ", startTime, endTime);

    startTime = std::chrono::high_resolution_clock::now();
    for_each(std::execution::seq, ints.cbegin(), ints.cend(), long_function);
    endTime = std::chrono::high_resolution_clock::now();
    // printf("STL-seq : start: %g end: %g Time: %fms\n", startTime, endTime,
    //    std::chrono::duration_cast<std::chrono::duration<double, std::milli>>(endTime - startTime).count());
    std::cout << "Begin: " << ints.front() << " End: " << ints.back() << " ";
    print_results("STL-seq : ", startTime, endTime);

    startTime = std::chrono::high_resolution_clock::now();
    std::for_each(std::execution::par, ints.cbegin(), ints.cend(), long_function);
    endTime = std::chrono::high_resolution_clock::now();
    // printf("STL-par : start: %g end: %g Time: %fms\n", startTime, endTime,
    //        std::chrono::duration_cast<std::chrono::duration<double, std::milli>>(endTime - startTime).count());
    std::cout << "Begin: " << ints.front() << " End: " << ints.back() << " ";
    print_results("STL-par : ", startTime, endTime);

    startTime = std::chrono::high_resolution_clock::now();
    parallel_for_each_pt(ints.cbegin(), ints.cend(), long_function);
    endTime = std::chrono::high_resolution_clock::now();
    // printf("Parallel package task : start: %g end: %g Time: %fms\n", startTime, endTime,
    //        std::chrono::duration_cast<std::chrono::duration<double, std::milli>>(endTime - startTime).count());
    std::cout << "Begin: " << ints.front() << " End: " << ints.back() << " ";
    print_results("Parallel package task : ", startTime, endTime);

    startTime = std::chrono::high_resolution_clock::now();
    parallel_for_each_async(ints.cbegin(), ints.cend(), long_function);
    endTime = std::chrono::high_resolution_clock::now();
    // printf("Parallel async : start: %g end: %g Time: %fms\n", startTime, endTime,
    //        std::chrono::duration_cast<std::chrono::duration<double, std::milli>>(endTime - startTime).count());
    std::cout << "Begin: " << ints.front() << " End: " << ints.back() << " ";
    print_results("Parallel async ", startTime, endTime);
}

int main()
{
    one_million_sort();
    one_million_quick_sort();
    one_million_parallel_for();
    return 0;
}
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
#include <vector>

const size_t testSize = 1000000;
const int iterationCount = 5;

void print_results(const char* const tag, const std::vector<double>& sorted,
                   std::chrono::high_resolution_clock::time_point startTime,
                   std::chrono::high_resolution_clock::time_point endTime)
{
    printf("%s: Lowest: %g Highest: %g Time: %fms\n", tag, sorted.front(), sorted.back(),
           std::chrono::duration_cast<std::chrono::duration<double, std::milli>>(endTime - startTime).count());
}

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
        print_results("Serial STL", sorted, startTime, endTime);
    }
    for (int i = 0; i < iterationCount; ++i) {
        std::vector<double> sorted(doubles);
        const auto startTime = std::chrono::high_resolution_clock::now();
        std::sort(std::execution::par, sorted.begin(), sorted.end());
        const auto endTime = std::chrono::high_resolution_clock::now();
        print_results("Parallel STL", sorted, startTime, endTime);
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
        printf("Serial quick sort: Lowest: %g Highest: %g Time: %fms\n", sorted.begin(), sorted.end(),
               std::chrono::duration_cast<std::chrono::duration<double, std::milli>>(endTime - startTime).count());
    }
    for (int i = 0; i < iterationCount; ++i) {
        std::list<double> sorted(doubles);
        const auto startTime = std::chrono::high_resolution_clock::now();
        parallel_quick_sort(sorted);
        const auto endTime = std::chrono::high_resolution_clock::now();
        printf("Parallel quick sort: Lowest: %g Highest: %g Time: %fms\n", sorted.begin(), sorted.end(),
               std::chrono::duration_cast<std::chrono::duration<double, std::milli>>(endTime - startTime).count());
    }
}

int main()
{
    one_million_sort();
    one_million_quick_sort();
    return 0;
}
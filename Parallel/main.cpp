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
        std::cout << "Begin: " << sorted.front() << " End: " << sorted.back() << " ";
        print_results("Serial quick sort", startTime, endTime);
    }
    for (int i = 0; i < iterationCount; ++i) {
        // very slow alg, simply return;
        return;
        std::list<double> sorted(doubles);
        const auto startTime = std::chrono::high_resolution_clock::now();
        parallel_quick_sort(sorted);
        const auto endTime = std::chrono::high_resolution_clock::now();
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

    // divide task to futures threads, leave one for this thread
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
    // last block for this thread
    std::for_each(block_start, last, f);
    // wait until future is ready
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
    std::cout << "Begin: " << ints.front() << " End: " << ints.back() << " ";
    print_results("STL-seq : ", startTime, endTime);

    startTime = std::chrono::high_resolution_clock::now();
    std::for_each(std::execution::par, ints.cbegin(), ints.cend(), long_function);
    endTime = std::chrono::high_resolution_clock::now();
    std::cout << "Begin: " << ints.front() << " End: " << ints.back() << " ";
    print_results("STL-par : ", startTime, endTime);

    startTime = std::chrono::high_resolution_clock::now();
    parallel_for_each_pt(ints.cbegin(), ints.cend(), long_function);
    endTime = std::chrono::high_resolution_clock::now();
    std::cout << "Begin: " << ints.front() << " End: " << ints.back() << " ";
    print_results("Parallel package task : ", startTime, endTime);

    startTime = std::chrono::high_resolution_clock::now();
    parallel_for_each_async(ints.cbegin(), ints.cend(), long_function);
    endTime = std::chrono::high_resolution_clock::now();
    std::cout << "Begin: " << ints.front() << " End: " << ints.back() << " ";
    print_results("Parallel async ", startTime, endTime);
}

template <typename Iterator, typename MatchType>
Iterator parallel_find_pt(Iterator first, Iterator last, MatchType match)
{
    struct find_element {
        void operator()(Iterator begin, Iterator end, MatchType match, std::promise<Iterator>* result,
                        std::atomic<bool>* done_flag)
        {
            try {
                for (; (begin != end) && !std::atomic_load(done_flag); ++begin) {
                    if (*begin == match) {
                        result->set_value(begin);
                        std::atomic_store(done_flag, true);
                        return;
                    }
                }
            } catch (...) {
                result->set_exception(std::current_exception());
                done_flag->store(true);
            }
        }
    };

    unsigned long const length = std::distance(first, last);
    if (!length) return last;

    unsigned long const min_per_thread = 25;
    unsigned long const max_threads = (length + min_per_thread - 1) / min_per_thread;
    unsigned long const hardware_threads = std::thread::hardware_concurrency();
    unsigned long const num_threads = std::min(hardware_threads != 0 ? hardware_threads : 2, max_threads);
    unsigned long const block_size = length / num_threads;

    std::promise<Iterator> result;
    std::atomic<bool> done_flag(false);

    std::vector<std::thread> threads(num_threads - 1);

    {
        join_threads joiner(threads);

        Iterator block_start = first;
        for (unsigned long i = 0; i < (num_threads - 1); i++) {
            Iterator block_end = block_start;
            std::advance(block_end, block_size);
            threads[i] = std::thread(find_element(), block_start, block_end, match, &result, &done_flag);
            block_start = block_end;
        }

        find_element()(block_start, last, match, &result, &done_flag);
    }

    if (!done_flag.load()) return last;
    return result.get_future().get();
}

template <typename Iterator, typename MatchType>
Iterator parallel_find_async(Iterator first, Iterator last, MatchType match, std::atomic<bool>* done_flag)
{
    try {
        unsigned long const length = std::distance(first, last);
        unsigned long const min_per_thread = 25;

        if (length < 2 * min_per_thread) {
            for (; (first != last) && !std::atomic_load(done_flag); ++first) {
                if (*first == match) {
                    std::atomic_store(done_flag, true);
                    return first;
                }
            }
            return last;
        } else {
            Iterator const mid_point = first + length / 2;
            std::future<Iterator> async_result
                = std::async(&parallel_find_async<Iterator, MatchType>, mid_point, last, match, std::ref(done_flag));
            Iterator const direct_result = parallel_find_async(first, mid_point, match, done_flag);
            return (direct_result == mid_point) ? async_result.get() : direct_result;
        }

    } catch (const std::exception&) {
        std::atomic_store(done_flag, true);
        throw;
    }
}

void one_million_parallel_find()
{
    std::vector<int> ints(testSize);
    for (size_t i = 0; i < testSize; i++) {
        ints[i] = i;
    }
    int looking_for = 5000000;

    auto startTime = std::chrono::high_resolution_clock::now();
    parallel_find_pt(ints.begin(), ints.end(), looking_for);
    auto endTime = std::chrono::high_resolution_clock::now();
    std::cout << "Begin: " << ints.front() << " End: " << ints.back() << " ";
    print_results("Parallel find future: ", startTime, endTime);

    startTime = std::chrono::high_resolution_clock::now();
    std::find(ints.begin(), ints.end(), looking_for);
    endTime = std::chrono::high_resolution_clock::now();
    std::cout << "Begin: " << ints.front() << " End: " << ints.back() << " ";
    print_results("STL sequential find: ", startTime, endTime);

    startTime = std::chrono::high_resolution_clock::now();
    std::find(std::execution::seq, ints.begin(), ints.end(), looking_for);
    endTime = std::chrono::high_resolution_clock::now();
    std::cout << "Begin: " << ints.front() << " End: " << ints.back() << " ";
    print_results("STL parallel-seq find: ", startTime, endTime);

    startTime = std::chrono::high_resolution_clock::now();
    std::find(std::execution::par, ints.begin(), ints.end(), looking_for);
    endTime = std::chrono::high_resolution_clock::now();
    std::cout << "Begin: " << ints.front() << " End: " << ints.back() << " ";
    print_results("STL parallel-par find: ", startTime, endTime);

    std::atomic<bool> done_flag(false);
    startTime = std::chrono::high_resolution_clock::now();
    parallel_find_async(ints.begin(), ints.end(), looking_for, &done_flag);
    endTime = std::chrono::high_resolution_clock::now();
    std::cout << "Begin: " << ints.front() << " End: " << ints.back() << " ";
    print_results("Parallel find async: ", startTime, endTime);
}

template <typename Iterator, typename OutIterator>
void sequential_partial_sum(Iterator first, Iterator last, OutIterator y)
{
    unsigned long const length = std::distance(first, last);
    y[0] = first[0];
    for (size_t i = 1; i < length; i++) {
        y[i] = first[i] + y[i - 1];
    }
}

template <typename Iterator>
void parallel_partial_sum(Iterator first, Iterator last)
{
    typedef typename Iterator::value_type value_type;

    struct process_chunk {
        void operator()(Iterator begin, Iterator last, std::future<value_type>* previous_end_value,
                        std::promise<value_type>* end_value)
        {
            try {
                Iterator end = last;
                ++end;
                std::partial_sum(begin, end, begin);
                if (previous_end_value) {
                    auto addend = previous_end_value->get();
                    *last += addend;
                    if (end_value) {
                        end_value->set_value(*last);
                    }
                    std::for_each(begin, last, [addend](value_type& item) { item += addend; });
                } else if (end_value) {
                    end_value->set_value(*last);
                }
            } catch (...) {
                if (end_value) {
                    end_value->set_exception(std::current_exception());
                } else {
                    throw;
                }
            }
        }
    };

    unsigned long const length = std::distance(first, last);
    if (!length) return;

    unsigned long const min_per_thread = 25;
    unsigned long const max_threads = (length + min_per_thread - 1) / min_per_thread;
    unsigned long const hardware_threads = std::thread::hardware_concurrency();
    unsigned long const num_threads = std::min(hardware_threads != 0 ? hardware_threads : 2, max_threads);
    unsigned long const block_size = length / num_threads;

    std::vector<std::thread> threads(num_threads - 1);
    // i == 0 future is 0, first block only have promise
    std::vector<std::promise<value_type>> end_values(num_threads - 1);
    // last block only have future, promise = 0
    std::vector<std::future<value_type>> previous_end_values;
    previous_end_values.reserve(num_threads - 1);

    join_threads joiner(threads);

    Iterator block_start = first;
    for (unsigned long i = 0; i < (num_threads - 1); ++i) {
        Iterator block_last = block_start;
        std::advance(block_last, block_size - 1);
        threads[i] = std::thread(process_chunk(), block_start, block_last, (i != 0) ? &previous_end_values[i - 1] : 0,
                                 &end_values[i]);
        block_start = block_last;
        ++block_start;
        previous_end_values.push_back(end_values[i].get_future());
    }

    Iterator final_element = block_start;
    std::advance(final_element, std::distance(block_start, last) - 1);
    process_chunk()(block_start, final_element, (num_threads > 1) ? &previous_end_values.back() : 0, 0);
}

void sum_ten_nums()
{
    std::vector<int> ints(10);
    std::vector<int> outs(10);
    for (size_t i = 0; i < ints.size(); i++) {
        ints[i] = i;
    }
    auto startTime = std::chrono::high_resolution_clock::now();
    sequential_partial_sum(ints.begin(), ints.end(), outs.begin());
    auto endTime = std::chrono::high_resolution_clock::now();
    std::cout << "Begin: " << ints.front() << " End: " << ints.back() << " ";
    print_results("sequential_partial_sum: ", startTime, endTime);

    for (size_t i = 0; i < outs.size(); i++) {
        std::cout << outs[i] << " ";
    }
    std::cout << std::endl;
}

void one_million_sum_nums()
{
    std::vector<int> ints(testSize);
    std::vector<int> outs(testSize);
    for (auto& i : ints) {
        i = 1;
    }
    auto startTime = std::chrono::high_resolution_clock::now();
    std::inclusive_scan(ints.begin(), ints.end(), outs.begin());
    auto endTime = std::chrono::high_resolution_clock::now();
    std::cout << "Begin: " << ints.front() << " End: " << ints.back() << " ";
    print_results("sequential inclusive_scan: ", startTime, endTime);

    startTime = std::chrono::high_resolution_clock::now();
    std::inclusive_scan(std::execution::par, ints.cbegin(), ints.cend(), outs.begin());
    endTime = std::chrono::high_resolution_clock::now();
    std::cout << "Begin: " << ints.front() << " End: " << ints.back() << " ";
    print_results("parallel inclusive_scan: ", startTime, endTime);

    startTime = std::chrono::high_resolution_clock::now();
    parallel_partial_sum(ints.begin(), ints.end());
    endTime = std::chrono::high_resolution_clock::now();
    std::cout << "Begin: " << ints.front() << " End: " << ints.back() << " ";
    print_results("parallel parallel_partial_sum: ", startTime, endTime);
}

class Matrix {
    int* data;
    int rows;
    int columns;

public:
    // row major format
    Matrix(int _n, int _m)
        : rows(_n)
        , columns(_m)
    {
        data = new int[rows * columns];
        std::fill(data, data + rows * columns, 0);
    }

    void set_value(int i, int j, int value) { data[i * columns + j] = value; }

    void set_all(int value) { std::fill(data, data + rows * columns, value); }

    // index in results matrix: row = index/columns, column = index % columns
    static void multiply(Matrix* x, Matrix* y, Matrix* results)
    {
        if (!(x->columns == y->rows) || !((x->rows == results->rows) && (y->columns == results->columns))) {
            std::cout << "Error: Invalid dimension of matrix for multiplication" << std::endl;
        }
        int r = results->rows * results->columns;
        for (size_t i = 0; i < r; i++) {
            for (size_t j = 0; j < x->columns; j++) {
                results->data[i] += x->data[(i / results->columns) * x->columns + j]
                                    * y->data[i % results->columns + j * y->columns];
            }
        }
    }

    void print()
    {
        for (int j = 0; j < rows; j++) {
            for (int i = 0; i < columns; i++) {
                std::cout << data[0] << " ";
            }
            std::cout << std::endl;
        }
    }
};

void showMatrix()
{
    Matrix A(3, 4);
    Matrix B(4, 5);
    Matrix results(3, 5);

    A.set_all(1);
    B.set_all(1);

    Matrix::multiply(&A, &B, &results);

    results.print();
}

int main()
{
    one_million_sort();
    one_million_quick_sort();
    one_million_parallel_for();
    one_million_parallel_find();
    sum_ten_nums();
    one_million_sum_nums();
    showMatrix();
    return 0;
}
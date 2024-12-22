#include "common.h"
#include <chrono>
#include <condition_variable>
#include <functional>
#include <future>
#include <iostream>
#include <list>
#include <mutex>
#include <numeric>
#include <queue>
#include <stack>
#include <thread>
#include <vector>

bool have_i_arrived = false;
int distance_my_destination = 10;
int distance_coverd = 0;
std::condition_variable cv;
std::mutex m;

bool keep_driving()
{
    while (true) {
        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
        distance_coverd++;
        // notify when events occurs
        if (distance_coverd == distance_my_destination) cv.notify_one();
        if (distance_coverd > 20) return true;
    }
    return false;
}

void keep_awake_all_night()
{
    while (distance_coverd < distance_my_destination) {
        std::cout << "keep check, whether i am there \n";
        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    }
    std::cout << "finally i am there, awake all night, distance_coverd = " << distance_coverd << std::endl;
}

void ask_driver_to_wake_u_up_at_right_time()
{
    std::unique_lock<std::mutex> ul(m);
    // need provide a lock and a condition to check after wake up
    // wake up can be due to notification from another thread or spurious wake
    // if condition is false, this thread will call unlock and goto sleep.
    // if condition is true, this thread will call lock and proceed until thread finish.
    // only unique_lock can provide that flexibility, so not lock_guard.
    cv.wait(ul, [] { return distance_coverd == distance_my_destination; });
    std::cout << "finally i am there, notify by driver, distance_coverd = " << distance_coverd << std::endl;
}

void set_the_alarm_and_take_a_nap()
{
    if (distance_coverd < distance_my_destination) {
        std::cout << "let me take a nap\n";
        std::this_thread::sleep_for(std::chrono::milliseconds(10000));
    }
    std::cout << "finally i am there, take a nap, distance_coverd = " << distance_coverd << std::endl;
}

void run_code()
{
    std::thread driver_thread(keep_driving);
    std::thread keep_awake_all_night_thread(keep_awake_all_night);
    std::thread set_the_alarm_and_take_a_nap_thread(set_the_alarm_and_take_a_nap);
    std::thread passenger_thread(ask_driver_to_wake_u_up_at_right_time);

    keep_awake_all_night_thread.join();
    set_the_alarm_and_take_a_nap_thread.join();
    driver_thread.join();
    passenger_thread.join();
}

template <typename T>
class thread_safe_queue {
    std::mutex m;
    std::condition_variable cv;
    // store pointer instead of raw value
    std::queue<std::shared_ptr<T>> queue;

public:
    thread_safe_queue() {}

    thread_safe_queue(thread_safe_queue const& other)
    {
        std::lock_guard<std::mutex> lg(other.m);
        queue = other.queue;
    }

    void push(T value)
    {
        std::lock_guard<std::mutex> lg(m);
        queue.push(std::make_shared<T>(value));
        cv.notify_one();
    }

    bool pop(T& ref)
    {
        std::lock_guard<std::mutex> lg(m);
        if (queue.empty()) {
            return false;
        } else {
            ref = queue.front();
            queue.pop();
            return true;
        }
    }

    std::shared_ptr<T> pop()
    {
        std::lock_guard<std::mutex> lg(m);
        if (queue.empty()) {
            return std::shared_ptr<T>();
        } else {
            std::shared_ptr<T> ref(queue.front());
            queue.pop();
            return ref;
        }
    }

    bool wait_pop(T& ref)
    {
        std::unique_lock<std::mutex> lg(m);
        cv.wait(lg, [this] { return !queue.empty(); });
        ref = *(queue.front().get());
        queue.pop();
        return true;
    }

    std::shared_ptr<T> wait_pop()
    {
        std::unique_lock<std::mutex> lg(m);
        cv.wait(lg, [this] { return !queue.empty(); });
        std::shared_ptr<T> ref = queue.front();
        queue.pop();
        return ref;
    }

    bool empty()
    {
        std::lock_guard<std::mutex> lg(m);
        return queue.empty();
    }

    size_t size()
    {
        std::lock_guard<std::mutex> lg(m);
        return queue.size();
    }
};

void run_thread_safe_queue()
{
    thread_safe_queue<int> queue;
    std::thread thread_1([&] {
        int value;
        queue.wait_pop(value);
        std::cout << "value from thread 1 -- " << value << std::endl;
    });
    std::thread thread_2([&] {
        int x = 10;
        std::this_thread::sleep_for(std::chrono::milliseconds(2000));
        queue.push(x);
    });
    thread_1.join();
    thread_2.join();
}

int find_answer_how_old_universe_is()
{
    // this is not the true value
    return 5000;
}

void do_other_calculations() { std::cout << "Doing other stuff" << std::endl; }

void run_asnync()
{
    std::future<int> the_answer_future = std::async(find_answer_how_old_universe_is);
    do_other_calculations();
    std::cout << "The answer is " << the_answer_future.get() << std::endl;
}

void printing() { std::cout << "printing runs on-" << std::this_thread::get_id() << std::endl; }

int addition(int x, int y)
{
    std::cout << "addition runs on-" << std::this_thread::get_id() << std::endl;
    return x + y;
}

int substract(int x, int y)
{
    std::cout << "substract runs on-" << std::this_thread::get_id() << std::endl;
    return x - y;
}

void run_future()
{
    std::cout << "main thread id -" << std::this_thread::get_id() << std::endl;
    int x = 100;
    int y = 50;
    // seperate thread
    std::future<void> f1 = std::async(std::launch::async, printing);
    // same thread
    std::future<int> f2 = std::async(std::launch::deferred, addition, x, y);
    std::future<int> f3 = std::async(std::launch::deferred | std::launch::async, substract, x, y);

    f1.get();
    std::cout << "value recieved using f2 future - " << f2.get() << std::endl;
    std::cout << "value recieved using f3 future - " << f3.get() << std::endl;
}

const int MIN_ELEMENT_COUNT = 1000;
template <typename iterator>
int parallel_accumulate(iterator begin, iterator end)
{
    long length = std::distance(begin, end);
    if (length <= MIN_ELEMENT_COUNT) {
        std::cout << std::this_thread::get_id() << " " << length << std::endl;
        return std::accumulate(begin, end, 0);
    }
    iterator mid = begin;
    std::advance(mid, (length + 1) / 2);

    // recursive all to parallel
    std::future<int> f1
        = std::async(std::launch::deferred | std::launch::async, parallel_accumulate<iterator>, mid, end);
    std::future<int> f2
        = std::async(std::launch::deferred | std::launch::async, parallel_accumulate<iterator>, begin, mid);
    return f1.get() + f2.get();
}

void run_parallel_accumulate_with_async()
{
    std::vector<int> v(10000, 1);
    std::cout << "The sum is " << parallel_accumulate(v.begin(), v.end()) << std::endl;
}

int add(int x, int y)
{
    std::this_thread::sleep_for(std::chrono::milliseconds(500));
    std::cout << "add function runs in : " << std::this_thread::get_id() << std::endl;
    return x + y;
}

void task_same_thread()
{
    std::packaged_task<int(int, int)> task_1(add);
    std::future<int> future_1 = task_1.get_future();
    task_1(7, 8);
    std::cout << "task same thread - " << future_1.get() << std::endl;
}

void task_another_thread()
{
    std::packaged_task<int(int, int)> task_1(add);
    std::future<int> future_1 = task_1.get_future();
    std::thread thread_1(std::move(task_1), 5, 6);
    thread_1.detach();
    std::cout << "task another thread - " << future_1.get() << std::endl;
}

void print_int(std::future<int>& fut)
{
    std::cout << "waiting for value from print thread \n";
    std::cout << "value: " << fut.get() << std::endl;
}

void run_promise()
{
    std::promise<int> prom;
    std::future<int> fut = prom.get_future();
    std::thread print_thread(print_int, std::ref(fut));
    std::this_thread::sleep_for(std::chrono::milliseconds(5000));
    std::cout << "setting the value in main thread\n";
    prom.set_value(10);
    print_thread.join();
}

int main()
{
    run_code();
    run_thread_safe_queue();
    run_asnync();
    run_future();
    run_parallel_accumulate_with_async();
    task_same_thread();
    task_another_thread();
    run_promise();
    return 0;
}
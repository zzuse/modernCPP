// INSTALL: sudo port install gcc13
// COMPILE: /opt/local/bin/g++-mp-13 -std=c++20 -fcoroutines -pthread -o main main.cpp
// jthread only supported by gcc
#include <atomic>
#include <condition_variable>
#include <coroutine>
#include <functional>
#include <future>
#include <iostream>
#include <thread>
#include <vector>
using namespace ::std::literals;

void do_some_work() { std::cout << "Do some work \n" << std::endl; }

void do_some_thing1(std::stop_token token)
{
    int counter{0};
    while (counter < 10) {
        if (token.stop_requested()) {
            return;
        }
        std::this_thread::sleep_for(0.2s);
        std::cerr << "This is interruptible thread: " << std::this_thread::get_id() << counter << std::endl;
        ++counter;
    }
}

void do_something_else()
{
    int counter{0};
    while (counter < 10) {
        std::this_thread::sleep_for(0.2s);
        std::cerr << "This is non-interruptible thread : " << std::this_thread::get_id() << counter << std::endl;
        ++counter;
    }
}

class interrupt_flag {
    bool local_flag;

public:
    void set() { local_flag = true; }
    bool is_set() { return local_flag; }
};

thread_local interrupt_flag this_thread_flag;
class jthread_local {
    std::thread _thread;
    interrupt_flag* flag;

public:
    template <typename FunctionType>
    jthread_local(FunctionType f)
    {
        std::promise<interrupt_flag*> p;
        _thread = std::thread([f, &p] {
            p.set_value(&this_thread_flag);
            f();
        });
        flag = p.get_future().get();
    }
    void interrupt() { flag->set(); }
    ~jthread_local()
    {
        if (_thread.joinable()) _thread.join();
    }
};

bool interrupt_point()
{
    if (this_thread_flag.is_set()) return true;
    return false;
}

void do_some_thing2()
{
    int counter{0};
    while (counter < 10) {
        if (interrupt_point()) {
            return;
        }
        std::this_thread::sleep_for(0.2s);
        std::cerr << "This is interruptible thread: " << std::this_thread::get_id() << counter << std::endl;
        ++counter;
    }
}

#include <cassert>
class resumable {
public:
    struct promise_type;
    using coro_handle = std::coroutine_handle<promise_type>;
    resumable(coro_handle handle)
        : handle_(handle)
    {
        assert(handle);
    }
    resumable(resumable&) = delete;
    resumable(resumable&&) = delete;
    bool resume()
    {
        if (not handle_.done()) handle_.resume();
        return not handle_.done();
    }
    ~resumable() { handle_.destroy(); }

private:
    coro_handle handle_;
};

struct resumable::promise_type {
    using coro_handle = std::coroutine_handle<promise_type>;
    auto get_return_object() noexcept { return coro_handle::from_promise(*this); }
    auto initial_suspend() noexcept { return std::suspend_always(); }
    auto final_suspend() noexcept { return std::suspend_always(); }
    void return_void() noexcept {}
    void unhandled_exception() noexcept { std::terminate(); }
};

resumable foo()
{
    std::cout << "a" << std::endl;
    co_await std::suspend_always();
    std::cout << "b" << std::endl;
    co_await std::suspend_always();
    std::cout << "c" << std::endl;
}

std::vector<int> getNumbers(int begin, int end, int inc = 1)
{
    std::vector<int> numbers;
    for (int i = begin; i < end; i += inc) {
        numbers.push_back(i);
    }
    return numbers;
}

template <typename T>
struct Generator {
    struct promise_type;
    using handle_type = std::coroutine_handle<promise_type>;
    handle_type coro;
    Generator(handle_type h)
        : coro(h)
    {
    }
    ~Generator()
    {
        if (coro) coro.destroy();
    }
    T getValue() { return coro.promise().current_value; }
    bool next()
    {
        coro.resume();
        return not coro.done();
    }
};

template <typename T>
struct Generator<T>::promise_type {
    using coro_handle = std::coroutine_handle<promise_type>;
    auto get_return_object() noexcept { return coro_handle::from_promise(*this); }
    auto initial_suspend() noexcept { return std::suspend_always(); }
    auto final_suspend() noexcept { return std::suspend_always(); }
    void return_void() noexcept {}
    void unhandled_exception() noexcept { std::terminate(); }
    auto yield_value(const T value)
    {
        current_value = value;
        return std::suspend_always{};
    }

    T current_value;
};

Generator<int> getNext(int start = 0, int step = 1) noexcept
{
    auto value = start;
    for (int i = 0;; ++i) {
        co_yield value;
        value += step;
    }
}

class barrier_spin {
    unsigned const thresh_hold;
    std::atomic<unsigned> count;
    std::atomic<unsigned> generation;

public:
    explicit barrier_spin(unsigned _thresh_hold)
        : thresh_hold(_thresh_hold)
        , count(_thresh_hold)
        , generation(0)
    {
    }

    void wait()
    {
        unsigned const my_generation = generation;
        if (!--count) {
            count = thresh_hold;
            ++generation;
        } else {
            while (generation == my_generation) std::this_thread::yield();
        }
    }
};

void func(barrier_spin& cur_barier, std::atomic<int>& counter)
{
    ++counter;
    cur_barier.wait();
    std::cout << "Current value after barrier: " << counter << "\n";
}

class barrier_cond {
public:
    explicit barrier_cond(std::size_t iCount)
        : mThreshold(iCount)
        , mCount(iCount)
        , mGeneration(0)
    {
    }

    void wait()
    {
        std::unique_lock<std::mutex> lLock{mMutex};
        auto lGen = mGeneration;
        if (!--mCount) {
            mGeneration++;
            mCount = mThreshold;
            mCond.notify_all();
        } else {
            mCond.wait(lLock, [this, lGen] { return lGen != mGeneration; });
        }
    }

private:
    std::mutex mMutex;
    std::condition_variable mCond;
    std::size_t mThreshold;
    std::size_t mCount;
    std::size_t mGeneration;
};

void func_cond(barrier_cond& cur_barier, std::atomic<int>& counter)
{
    ++counter;
    cur_barier.wait();
    std::cout << "Current value after barrier cond: " << counter << "\n";
}

int main()
{
    std::jthread thread1(do_some_work);
    std::jthread nonInterruptable1(do_something_else);
    std::jthread interruptable1(do_some_thing1);
    std::this_thread::sleep_for(1.0s);
    interruptable1.request_stop();
    nonInterruptable1.request_stop();

    std::cout << std::endl;
    jthread_local nonInterruptable2(do_something_else);
    jthread_local interruptable2(do_some_thing2);
    interruptable2.interrupt();
    nonInterruptable2.interrupt();

    resumable res1 = foo();
    res1.resume();
    std::cout << std::endl;

    const auto numbers = getNumbers(10, 30);
    for (int i = 0; i < 10; i++) std::cout << numbers[i] << " ";
    std::cout << std::endl;
    for (int i = 10; i < 20; i++) std::cout << numbers[i] << " ";
    std::cout << std::endl;

    res1.resume();
    res1.resume();

    auto gen = getNext();
    for (int i = 0; i <= 10; ++i) {
        gen.next();
        std::cout << " " << gen.getValue();
    }

    auto gen2 = getNext(100, -10);
    for (int i = 0; i <= 20; ++i) {
        gen2.next();
        std::cout << " " << gen2.getValue();
    }

    barrier_spin bar(3);
    std::atomic<int> current(0);
    std::thread thr1(std::bind(&func, std::ref(bar), std::ref(current)));
    std::thread thr2(std::bind(&func, std::ref(bar), std::ref(current)));
    std::thread thr3(std::bind(&func, std::ref(bar), std::ref(current)));
    thr1.join();
    thr2.join();
    thr3.join();

    std::cout << std::endl;

    barrier_cond bar_cond(3);
    std::atomic<int> current_cond(0);
    std::thread thr4(std::bind(&func_cond, std::ref(bar_cond), std::ref(current_cond)));
    std::thread thr5(std::bind(&func_cond, std::ref(bar_cond), std::ref(current_cond)));
    std::thread thr6(std::bind(&func_cond, std::ref(bar_cond), std::ref(current_cond)));
    thr4.join();
    thr5.join();
    thr6.join();
    return 0;
}
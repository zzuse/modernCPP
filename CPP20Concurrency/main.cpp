// INSTALL: sudo port install gcc13
// COMPILE: /opt/local/bin/g++-mp-13 -std=c++20 -o main main.cpp
// jthread only supported by gcc
#include <iostream>
#include <thread>
using namespace ::std::literals;

void do_some_work() { std::cout << "Do some work \n" << std::endl; }

void do_some_thing(std::stop_token token)
{
    int counter{0};
    while (counter < 10) {
        if (token.stop_requested()) {
            return;
        }
        std::this_thread::sleep_for(0.2s);
        std::cerr << "This is interruptible thread: " << counter << std::endl;
        ++counter;
    }
}

void do_something_else()
{
    int counter{0};
    while (counter < 10) {
        std::this_thread::sleep_for(0.2s);
        std::cerr << "This is non-interruptible thread : " << counter << std::endl;
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

int main()
{
    std::jthread thread1(do_some_work);
    std::jthread nonInterruptable(do_something_else);
    std::jthread interruptable(do_some_thing);
    std::this_thread::sleep_for(1.0s);
    interruptable.request_stop();
    nonInterruptable.request_stop();
    return 0;
}
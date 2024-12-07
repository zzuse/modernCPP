#include "common.h"
#include <iostream>
#include <list>
#include <mutex>
#include <stack>
#include <thread>

/*
 * lock - locks the mutex, blocks if the mutex is not available
 * try_lock - tries to lock the mutex, returns if the mutex is not available
 * unlock - unlocks the mutex
 */
class list_wrapper {
    std::list<int> my_list;
    std::mutex m;

public:
    void add_to_list(int const& x)
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

    // returning pointer or reference will put this data not thread safe.
    std::list<int>* get_data() { return &my_list; }
};

void run_code()
{
    list_wrapper my_list_wrapper;
    std::thread thread_1(&list_wrapper::add_to_list, &my_list_wrapper, 4);
    std::thread thread_2(&list_wrapper::add_to_list, &my_list_wrapper, 11);
    thread_1.join();
    thread_2.join();
    for (auto x : *(my_list_wrapper.get_data())) {
        std::cout << x << std::endl;
    }
}

class data_object {
public:
    int nothing;
};

class data_wrapper {
    data_object protected_data;
    std::mutex m;

public:
    template <typename function>
    void do_some_work(function f)
    {
        std::lock_guard<std::mutex> lg(m);
        // even protected data, we don't konw what that particular function would do
        f(protected_data);
    }
};

data_object* unprotected_data;
void malisious_function(data_object& data)
{
    // other threads can access this data pointer to do any work
    // so this case also not thread safe
    unprotected_data = &data;
    unprotected_data->nothing = 5;
}

void run_malisious_code()
{
    data_wrapper wrapper;
    wrapper.do_some_work(malisious_function);
    std::cout << unprotected_data->nothing << std::endl;
}

template <typename T>
class trivial_thread_safe_stack {
    std::stack<T> stk;
    std::mutex m;

public:
    void push(T element)
    {
        std::lock_guard<std::mutex> lg(m);
        stk.push(element);
    }

    void pop()
    {
        std::lock_guard<std::mutex> lg(m);
        stk.pop();
    }

    T& top()
    {
        std::lock_guard<std::mutex> lg(m);
        return stk.top();
    }

    bool empty()
    {
        std::lock_guard<std::mutex> lg(m);
        return stk.empty();
    }

    size_t size()
    {
        std::lock_guard<std::mutex> lg(m);
        return stk.size();
    }
};

void run_trivial_thread_safe_stack()
{
    trivial_thread_safe_stack<int> stk{};
    stk.push(1);
    stk.push(2);
    stk.push(3);
    std::thread thread_1([&stk] {
        if (!stk.empty()) {
            int value = stk.top();
            std::cout << "value from thread 1 --" << value << std::endl;
            stk.pop();
        }
    });
    std::thread thread_2([&stk] {
        if (!stk.empty()) {
            int value = stk.top();
            std::cout << "value from thread 2 --" << value << std::endl;
            stk.pop();
        }
    });
    thread_1.join();
    thread_2.join();
    std::cout << stk.top() << std::endl;
}

int main()
{
    run_code();
    run_malisious_code();
    run_trivial_thread_safe_stack();
    return 0;
}
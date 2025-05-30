#pragma once
#include <condition_variable>
#include <memory>
#include <mutex>
#include <queue>
#include <thread>
#include <vector>

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

template <typename T>
class threadsafe_queue {
private:
    mutable std::mutex mut;
    std::queue<std::shared_ptr<T>> data_queue;
    std::condition_variable data_cond;

public:
    threadsafe_queue() {}

    // Blocking.These methods block the calling thread until an element is available in the queue.
    void wait_and_pop(T& value)
    {
        std::unique_lock<std::mutex> lk(mut);
        data_cond.wait(lk, [this] { return !data_queue.empty(); });
        value = std::move(*data_queue.front());
        data_queue.pop();
    }
    // Non-Blocking.These methods attempt to pop an element but return immediately if the queue is empty.
    bool try_pop(T& value)
    {
        std::lock_guard<std::mutex> lk(mut);
        if (data_queue.empty()) return false;
        value = std::move(*data_queue.front());
        data_queue.pop();
        return true;
    }

    std::shared_ptr<T> wait_and_pop()
    {
        std::unique_lock<std::mutex> lk(mut);
        data_cond.wait(lk, [this] { return !data_queue.empty(); });
        std::shared_ptr<T> res = data_queue.front();
        data_queue.pop();
        return res;
    }

    std::shared_ptr<T> try_pop()
    {
        std::lock_guard<std::mutex> lk(mut);
        if (data_queue.empty()) return std::shared_ptr<T>();
        std::shared_ptr<T> res = data_queue.front();
        data_queue.pop();
        return res;
    }

    bool empty() const
    {
        std::lock_guard<std::mutex> lk(mut);
        return data_queue.empty();
    }

    void push(T new_value)
    {
        std::shared_ptr<T> data(std::make_shared<T>(std::move(new_value)));
        std::lock_guard<std::mutex> lk(mut);
        data_queue.push(data);
        data_cond.notify_one();
    }
};

// The function_wrapper class is a utility designed to encapsulate callable objects (such as functions, lambdas, or
// functors) in a type-erased, move-only wrapper. It provides a way to store and invoke any callable object in a uniform
// manner, making it particularly useful in scenarios like thread pools, where tasks of varying types need to be stored
// and executed.
class function_wrapper {
    struct impl_base {
        virtual void call() = 0;
        virtual ~impl_base() {}
    };

    template <typename F>
    struct impl_type : impl_base {
        F f;
        impl_type(F&& f_)
            : f(std::move(f_))
        {
        }
        void call() { f(); }
    };

    std::unique_ptr<impl_base> impl;

public:
    template <typename F>
    function_wrapper(F&& f)
        : impl(new impl_type<F>(std::move(f)))
    {
    }

    void operator()() { impl->call(); }

    function_wrapper() {}

    function_wrapper(function_wrapper&& other)
        : impl(std::move(other.impl))
    {
    }

    function_wrapper& operator=(function_wrapper&& other)
    {
        impl = std::move(other.impl);
        return *this;
    }

    function_wrapper(const function_wrapper&) = delete;
    function_wrapper(function_wrapper&) = delete;
};
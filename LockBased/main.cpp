#include "common.h"
#include <condition_variable>
#include <iostream>
#include <memory>
#include <mutex>
#include <queue>
#include <thread>

template <typename T>
class sequential_queue {
    struct node {
        T data;
        std::unique_ptr<node> next;
        node(T _data)
            : data(std::move(_data))
        {
        }
    };
    std::unique_ptr<node> head;
    node* tail;
    std::mutex head_mutex;
    std::mutex tail_mutex;

public:
    sequential_queue()
        : head(new node)
        , tail(head.get())
    {
    }

    void push(T value)
    {
        std::unique_ptr<node> new_node(new node(std::move(value)));
        node* const new_tail = new_node.get();

        std::lock_guard<std::mutex> tlg(tail_mutex);
        if (tail) {
            tail->next = std::move(new_node);
        } else {
            std::lock_guard<std::mutex> hlg(head_mutex);
            head = std::move(new_node);
        }
        tail = new_tail;
    }

    std::shared_ptr<T> pop()
    {
        // we have to lock both because push might operate on tail node when head and tail is the same node
        std::lock_guard<std::mutex> hlg(head_mutex);
        std::lock_guard<std::mutex> tlg(tail_mutex);
        if (!head) {
            return std::shared_ptr<T>();
        }
        std::shared_ptr<T> const res(std::make_shared<T>(std::move(head->data)));
        std::unique_ptr<node> const old_head = std::move(head);
        head = std::move(old_head->next);
        return res;
    }
};

template <typename T>
class sequential_safe_queue {
    struct node {
        std::shared_ptr<T> data;
        std::unique_ptr<node> next;

        node() {}
        node(T _data)
            : data(std::move(_data))
        {
        }
    };
    std::unique_ptr<node> head;
    node* tail;
    std::mutex head_mutex;
    std::mutex tail_mutex;
    std::condition_variable cv;

    node* get_tail()
    {
        std::lock_guard<std::mutex> lg(tail_mutex);
        return tail;
    }

    std::unique_ptr<node> wait_pop_head()
    {
        std::unique_lock<std::mutex> lock(head_mutex);
        cv.wait(lock, [&] { return head.get() != get_tail(); });
        std::unique_ptr<node> old_head = std::move(head);
        head = std::move(old_head->next);
        return old_head;
    }

public:
    sequential_safe_queue()
        : head(new node)
        , tail(head.get())
    {
    }

    void push(T value)
    {
        std::shared_ptr<T> new_data(std::make_shared<T>(std::move(value)));
        std::unique_ptr<node> p(new node);
        node* const new_tail = p.get();
        {
            std::lock_guard<std::mutex> lgt(tail_mutex);
            tail->data = new_data;
            tail->next = std::move(p);
            // new dummy node as new tail;
            tail = new_tail;
        }
        cv.notify_one();
    }

    std::shared_ptr<T> pop()
    {
        std::lock_guard<std::mutex> lg(head_mutex);
        if (head.get() == get_tail()) {
            return std::shared_ptr<T>();
        }
        std::shared_ptr<T> const res(head->data);
        std::unique_ptr<node> const old_head = std::move(head);
        head = std::move(old_head->next);
        return res;
    }

    std::shared_ptr<T> wait_pop()
    {
        std::unique_ptr<node> old_head = wait_pop_head();
        // no need std::move() because of copy ellision?
        return old_head ? old_head->data : std::shared_ptr<T>();
    }

    void printData();
};

template <typename T>
inline void sequential_safe_queue<T>::printData()
{
    if (head.get() == get_tail()) {
        std::cout << "Queue is empty..." << std::endl;
        return;
    }
    std::lock_guard<std::mutex> hlg(head_mutex);
    node* current = head.get();
    std::cout << "Queue from top..." << std::endl;
    int index{};
    while (current->data != nullptr) {
        std::cout << "current:" << current << ", value [" << index++ << "]:" << *(current->data) << std::endl;
        current = (current->next).get();
    }
    std::cout << "End of Queue..." << std::endl;
}

void run_code()
{
    sequential_safe_queue<int> queueInteger;
    queueInteger.push(5645);
    queueInteger.push(87456);
    queueInteger.push(94564);
    queueInteger.push(2347);
    queueInteger.push(634);
    queueInteger.printData();

    std::cout << "Removing: " << *(queueInteger.pop().get()) << std::endl;
    std::cout << "Removing: " << *(queueInteger.pop().get()) << std::endl;
    std::cout << "Removing by wait_pop: " << *(queueInteger.wait_pop().get()) << std::endl;
    queueInteger.printData();

    std::cout << "Removing by wait_pop: " << *(queueInteger.wait_pop().get()) << std::endl;
    queueInteger.printData();
    std::cout << "Removing: " << *(queueInteger.pop().get()) << std::endl;
    queueInteger.printData();
}

int main()
{
    run_code();
    return 0;
}
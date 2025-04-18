// INSTALL: sudo port install gcc13
// COMPILE: /opt/local/bin/g++-mp-13 -std=c++20 -fcoroutines -pthread -o main main.cpp
// jthread only supported by gcc
#include <assert.h>
#include <atomic>
#include <chrono>
#include <iostream>
#include <queue>
#include <thread>
#include <vector>

template <typename T>
class lock_free_stack {
private:
    struct node {
        std::shared_ptr<T> data;
        node* next;
        node(T const& _data)
            : data(std::make_shared<T>(_data))
        {
        }
    };
    std::atomic<node*> head;

public:
    void push(T const& _data)
    {
        node* const new_node = new node(_data);
        new_node->next = head.load();
        // the current value of the atomic variable (head in this case) with an expected value (new_node->next). If the
        // current value matches the expected value, it replaces the atomic variable's value with a new value (new_node)
        while (!head.compare_exchange_weak(new_node->next, new_node));
    }

    std::shared_ptr<T> pop()
    {
        node* old_head = head.load();
        while (old_head && !head.compare_exchange_weak(old_head, old_head->next));
        return old_head ? old_head->data : std::shared_ptr<T>();
    }
};

int main() { return 0; }
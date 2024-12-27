#include "common.h"
#include <condition_variable>
#include <memory>
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

void run_code() {}

int main()
{
    run_code();
    return 0;
}
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
    std::atomic<int> threads_in_pop;
    std::atomic<node*> to_be_deleted;

    void try_reclaim(node* old_head)
    {
        // Checking threads_in_pop: The threads_in_pop variable tracks the number of threads currently performing a pop
        // operation. If only one thread is in the pop operation (threads_in_pop == 1), it is safe to delete the
        // old_head node immediately because no other thread can be accessing it.

        // Handling the to_be_deleted List: If the current thread is the only one in pop, it also processes the
        // to_be_deleted list, which contains nodes that were deferred for deletion by other threads. The
        // to_be_deleted.exchange(nullptr) operation atomically retrieves the current list of deferred nodes and resets
        // the to_be_deleted pointer to nullptr.

        // Appending to to_be_deleted: If there are multiple threads in pop (threads_in_pop > 1), the old_head node
        // cannot be deleted immediately. Instead, it is added to the to_be_deleted list for deferred deletion. This is
        // done by setting old_head->next to point to the current to_be_deleted list and using an atomic
        // compare_exchange_weak loop to update the to_be_deleted pointer.

        // Traversing and Appending Nodes: If the claimed_list is not empty and needs to be appended back to
        // to_be_deleted, the function traverses the list to find the last node and links it to the current
        // to_be_deleted list. This is also done atomically using a compare_exchange_weak loop
        if (threads_in_pop == 1) {
            delete old_head;
            node* claimed_list = to_be_deleted.exchange(nullptr);
            if (!--threads_in_pop) {
                delete_nodes(claimed_list);
            } else if (claimed_list) {
                node* last = claimed_list;
                while (node* const next = last->next) {
                    last = next;
                }
                last->next = to_be_deleted;
                while (!to_be_deleted.compare_exchange_weak(last->next, claimed_list));
            }
        } else {
            // add to to_be_deleted
            old_head->next = to_be_deleted;
            while (!to_be_deleted.compare_exchange_weak(old_head->next, old_head));
            --threads_in_pop;
        }
    }

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
        ++threads_in_pop;
        node* old_head = head.load();
        while (old_head && !head.compare_exchange_weak(old_head, old_head->next));
        std::shared_ptr<T> res;
        if (old_head) {
            res.swap(old_head->data);
        }
        try_reclaim(old_head);
        return res;
    }
};

int main() { return 0; }
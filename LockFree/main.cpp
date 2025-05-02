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

template <typename T>
class lock_free_stack_hazard {
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
    static const int max_hazard_pointers = 100;

    // Hazard pointers are used to indicate that a thread is currently accessing a specific node, preventing it from
    // being deleted prematurely.
    struct hazard_pointer {
        std::atomic<std::thread::id> id;
        std::atomic<void*> pointer;
    };
    hazard_pointer hazard_pointers[max_hazard_pointers];

    // This class manages hazard pointers for the current thread. It ensures that each thread gets a unique hazard
    // pointer and releases it when no longer needed.
    class hp_manager {
        hazard_pointer* hp;

    public:
        hp_owner()
            : hp(nullptr)
        {
            for (unsigned i = 0; i < max_hazard_pointers; ++i) {

                std::thread::id old_id;
                if (hazard_pointers[i].id.compare_exchange_strong(old_id, std::this_thread::get_id())) {
                    hp = &hazard_pointers[i];
                    break;
                }
            }

            if (!hp) {
                throw std::runtime_error("no hazard pointers available");
            }
        }
        std::atomic<void*>& get_pointer() { return hp->pointer; }
        ~hp_owner()
        {
            hp->pointer.store(nullptr);
            hp->id.store(std::thread::id());
        }
    };

    // This function provides a thread-local hazard pointer for the current thread.
    std::atomic<void*>& get_hazard_pointer_for_current_thread()
    {
        static thread_local hp_manager hz_owner;
        return hz_owner.get_pointer();
    }
    // Checks if a node is still being accessed by any thread by iterating through all hazard pointers.
    bool any_outstanding_hazards(node* p)
    {
        for (unsigned i = 0; i < max_hazard_pointers; ++i) {
            if (hazard_pointers[i].pointer.load() == p) {
                return true;
            }
            return false;
        }
    }
    // Adds a node to a deferred reclamation list if it cannot be deleted immediately.
    void reclaim_later(node* _node)
    {
        _node->next = nodes_to_reclaim.load();
        while (!nodes_to_reclaim.compare_exchange_weak(_node->next, _node));
    }
    // Deletes nodes from the deferred list if they are no longer being accessed by any thread.
    void delete_nodes_with_no_hazards()
    {
        node* current = nodes_to_reclaim.exchange(nullptr);
        while (current) {
            node* const next = current->next;
            if (!any_outstanding_hazards(current)) {
                delete current;
            } else {
                reclaim_later(current);
            }
            current = next;
        }
    }

public:
    lock_free_stack_hazard() {}

    void push(T const& _data)
    {
        node* const new_node = new node(_data);
        new_node->next = head.load();
        while (!head.compare_exchange_weak(new_node->next, new_node));
    }

    void pop(T& result)
    {
        // Uses a hazard pointer to protect the node being accessed.
        std::atomic<void*>& hp = get_hazard_pointer_for_current_thread();
        node* old_head = head.load();
        do {
            hp.store(old_head);
        } while (old_head && !head.compare_exchange_strong(old_head, old_head->next));

        hp.store(nullptr);
        std::shared_ptr<T> res;
        if (old_head) {
            res.swap(old_head->data);

            if (any_outstanding_hazards(old_head)) {
                reclaim_later(old_head);
            } else {
                delete old_head;
            }
        }
    }
};

template <typename T>
class lock_free_stack_ref_counting {
private:
    struct node;
    struct node_wrapper {
        // This helps manage references from outside the stack, such as during push or pop operations
        int external_count;
        node* ptr;
    };
    struct node {
        std::shared_ptr<T> data;
        // Tracks references from within the stack (e.g., links between nodes).
        std::atomic<int> internal_count;
        node_wrapper next;
        node(T const& _data)
            : data(std::make_shared<T>(_data))
            , internal_count(0)
        {
        }
    };

    void increment_head_ref_count(node_wrapper& old_counter)
    {
        node_wrapper new_counter;
        do {
            new_counter = old_counter;
            ++new_counter.external_count;
        } while (!head.compare_exchange_strong(old_counter, new_counter));
        old_counter.external_count = new_counter.external_count;
    }

    std::atomic<node_wrapper> head;

public:
    ~lock_free_stack_ref_counting() {}
    void push(T const& data)
    {
        node_wrapper new_node;
        new_node.ptr = new node(data);
        new_node.external_count = 1;
        new_node.ptr->next = head.load();
        while (!head.compare_exchange_weak(new_node.ptr->next, new_node));
    }
    std::shared_ptr<T> pop()
    {
        node_wrapper old_head = head.load();
        for (;;) {
            increment_head_ref_count(old_head);
            node* const ptr = old_head.ptr;
            if (!ptr) {
                return std::shared_ptr<T>();
            }
            if (head.compare_exchange_strong(old_head, ptr->next)) {
                std::shared_ptr<T> res;
                res.swap(ptr->data);
                // The external_count is decremented by 2 because:
                // One reference is removed when the head pointer is updated.
                // Another reference is removed because the current thread is no longer accessing the node.
                int const current_external_count = old_head.external_count - 2;

                if (ptr->internal_count.fetch_add(current_external_count) == 0) {
                    delete ptr;
                }
                return res;
            } else if (ptr->internal_count.fetch_sub(1) == 1) {
                delete ptr;
            }
        }
    }
};

int main() { return 0; }
// INSTALL: sudo port install gcc13
// COMPILE: /opt/local/bin/g++-mp-13 -std=c++20 -fcoroutines -pthread -o main main.cpp
// jthread only supported by gcc
#include <atomic>
#include <chrono>
#include <iostream>
#include <thread>
#include <vector>

void run_flag()
{
    std::atomic_flag flag1 = true;
    std::atomic_flag flag2 = ATOMIC_FLAG_INIT;
    std::cout << "1. previous flag value: " << flag2.test_and_set() << std::endl;
    std::cout << "2. previous flag value: " << flag2.test_and_set() << std::endl;
    flag2.clear();
    std::cout << "3. previous flag value: " << flag2.test_and_set() << std::endl;
}

void run_bool()
{
    std::atomic<bool> flag_1;
    std::cout << "flag 1 = " << flag_1 << std::endl;

    // not compile because atomic_bool is not copyable and not assignable
    // std::atomic<bool> flag_2(flag_1);
    // std::atomic<bool> flag_3 = flag_1;

    bool non_atomic_bool = true;
    std::atomic<bool> flag_4(non_atomic_bool);
    std::cout << "flag 4 = " << flag_4 << std::endl;
    std::atomic<bool> flag_5 = non_atomic_bool;
    std::cout << "flag 5 = " << flag_5 << std::endl;

    std::atomic<bool> x(false);
    std::cout << "atomic boolean is implemented lock free - " << (x.is_lock_free() ? "yes" : "no") << std::endl;

    std::atomic<bool> y(true);

    x.store(false);
    x.store(y);

    std::cout << "value of the atomic bool y - " << y.load() << std::endl;
    bool z = x.exchange(false);
    std::cout << "current value of atomic bool x - " << x.load() << std::endl;
    std::cout << "previous value of atomic bool x - " << z << std::endl;
}

void run_compare_exchange()
{
    std::atomic<int> x(20);
    int expected_value = 20;
    std::cout << "previous expected_value - " << expected_value << std::endl;
    bool return_val = x.compare_exchange_weak(expected_value, 6);
    std::cout << "operation successful   - " << (return_val ? "yes" : "no") << std::endl;
    std::cout << "current expected_value - " << expected_value << std::endl;
    std::cout << "current x              - " << x.load() << std::endl;
}

void run_atomic_pointer()
{
    int values[20];
    for (int i = 0; i < 20; i++) values[i] = i + 1;
    std::atomic<int*> x_pointer = values;
    std::cout << "atomic integer pointer lock free ? " << (x_pointer.is_lock_free() ? "yes" : "no") << std::endl;
    std::cout << "1. After initialiation value pointed by atomic pointer " << *x_pointer << std::endl;
    int* prev_pointer_val_1 = x_pointer.fetch_add(12);
    std::cout << "2. After fetch add previous value pointed by atomic pointer - " << *prev_pointer_val_1 << std::endl;
    std::cout << "2. After fetch add new value pointed by atomic pointer      - " << *x_pointer << std::endl;

    int* prev_pointer_val_2 = x_pointer.fetch_sub(3);
    std::cout << "3. After fetch sub previous value pointed by atomic pointer - " << *prev_pointer_val_2 << std::endl;
    std::cout << "3. After fetch add new value pointed by atomic pointer - " << *x_pointer << std::endl;

    x_pointer++;
    std::cout << "4. After post incr new value pointed by atomic pointer - " << *x_pointer << std::endl;

    x_pointer--;
    std::cout << "5. After post decr new value pointed by atomic pointer - " << *x_pointer << std::endl;

    int* y_pointer = values + 3;
    x_pointer.store(y_pointer);
    std::cout << "value referencing to by pointer : " << *(x_pointer.load()) << std::endl;

    bool ret_val = x_pointer.compare_exchange_weak(y_pointer, values + 10);
    std::cout << "store operation successful : " << (ret_val ? "yes" : "no") << std::endl;
    std::cout << "new value pointer by atomic pointer : " << *x_pointer << std::endl;
}

void reader_func() {}
void writer_func() {}

void run_happen_before()
{
    std::thread reader_thread(reader_func);
    std::thread writer_thread(writer_func);

    reader_thread.join();
    writer_thread.join();
}

int main()
{
    run_flag();
    run_bool();
    run_compare_exchange();
    run_atomic_pointer();
    run_happen_before();
    return 0;
}
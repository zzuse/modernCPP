// INSTALL: sudo port install gcc13
// COMPILE: /opt/local/bin/g++-mp-13 -std=c++20 -fcoroutines -pthread -o main main.cpp
// jthread only supported by gcc
#include <assert.h>
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

std::atomic<bool> data_ready = false;
std::vector<int> data_vector;
void reader_func()
{
    while (!data_ready) {
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
    }
    std::cout << "ready" << std::endl;
    std::cout << data_vector[0] << std::endl;
}
void writer_func()
{
    data_vector.push_back(3);
    std::cout << "not ready" << std::endl;
    data_ready.store(true);
}

void run_happen_before()
{
    std::thread reader_thread(reader_func);
    std::thread writer_thread(writer_func);

    reader_thread.join();
    writer_thread.join();
}

void run_mem_order()
{
    /*
       memory_order_seq_cst make sure different thread have same view of order
       memory_order_release and memory_order_acquire make sure thread release happen before thread acquire
       memory_order_relaxed make sure in the same thread have same order
       memory_order_consume is a special case of memory_order_acquire, depend operand relation of memory_order_release
    */
    std::atomic<int> x;
    x.store(5, std::memory_order_seq_cst);
    x.store(10, std::memory_order_release);
    x.load(std::memory_order_acquire);

    int value = 11;
    bool ret_val = x.compare_exchange_weak(value, 13, std::memory_order_release, std::memory_order_relaxed);
}

std::atomic<bool> x_0, y_0;
std::atomic<int> z_0;
void write_x0() { x_0.store(true, std::memory_order_seq_cst); }
void write_y0() { y_0.store(true, std::memory_order_seq_cst); }
void read_x0_then_y0()
{
    // loop until x is true
    while (!x_0.load(std::memory_order_seq_cst));

    // check wether y is true
    if (y_0.load(std::memory_order_seq_cst)) {
        z_0++;
    }
}
void read_y0_then_x0()
{
    // loop until y is true
    while (!y_0.load(std::memory_order_seq_cst));

    // check wether x is true
    if (x_0.load(std::memory_order_seq_cst)) {
        z_0++;
    }
}

void run_memory_order_seq_cst()
{
    x_0 = false;
    y_0 = false;
    z_0 = 0;
    std::thread thread_a(write_x0);
    std::thread thread_b(write_y0);
    std::thread thread_c(read_x0_then_y0);
    std::thread thread_d(read_y0_then_x0);

    thread_a.join();
    thread_b.join();
    thread_c.join();
    thread_d.join();

    assert(z_0 != 0);
    std::cout << z_0 << std::endl;
}

std::atomic<bool> x_1, y_1;
std::atomic<int> z_1;

void write_x1_then_y1()
{
    x_1.store(true, std::memory_order_relaxed);
    y_1.store(true, std::memory_order_relaxed);
}

void read_y1_then_x1()
{
    while (!y_1.load(std::memory_order_relaxed));
    if (x_1.load(std::memory_order_relaxed)) {
        z_1++;
    }
}

void run_memory_order_relaxed()
{
    x_1 = false;
    y_1 = false;
    z_1 = 0;

    std::thread writer_thread(write_x1_then_y1);
    std::thread reader_thread(read_y1_then_x1);

    writer_thread.join();
    reader_thread.join();
    // it may fire not sure
    assert(z_1 != 0);
    std::cout << z_1 << std::endl;
}

std::atomic<bool> x_2, y_2;
std::atomic<int> z_2;

void write_x2_then_y2()
{
    x_2.store(true, std::memory_order_relaxed);
    y_2.store(true, std::memory_order_release);
}

void read_y2_then_x2()
{
    while (!y_2.load(std::memory_order_acquire));
    if (x_2.load(std::memory_order_relaxed)) {
        z_2++;
    }
}

void run_memory_order_release()
{
    x_2 = false;
    y_2 = false;
    z_2 = 0;

    std::thread writer_thread(write_x2_then_y2);
    std::thread reader_thread(read_y2_then_x2);

    writer_thread.join();
    reader_thread.join();

    assert(z_2 != 0);
    std::cout << z_2 << std::endl;
}

std::atomic<bool> x_3, y_3;
std::atomic<int> z_3;

void write_x3() { x_3.store(true, std::memory_order_release); }

void write_y3() { y_3.store(true, std::memory_order_release); }

void read_x3_then_y3()
{
    while (!x_3.load(std::memory_order_acquire));

    if (y_3.load(std::memory_order_acquire)) {
        z_3++;
    }
}

void read_y3_then_x3()
{
    while (!y_3.load(std::memory_order_acquire));

    if (x_3.load(std::memory_order_acquire)) {
        z_3++;
    }
}

void run_memory_order_release_acquire()
{

    x_3 = false;
    y_3 = false;
    z_3 = 0;

    std::thread thread_a(write_x3);
    std::thread thread_b(write_y3);
    std::thread thread_c(read_x3_then_y3);
    std::thread thread_d(read_y3_then_x3);

    thread_a.join();
    thread_b.join();
    thread_c.join();
    thread_d.join();

    assert(z_3 != 0);
    std::cout << z_3 << std::endl;
}

std::atomic<int> data[5];
std::atomic<bool> sync1(false), sync2(false);

void thread_1_func()
{
    data[0].store(42, std::memory_order_relaxed);
    data[1].store(45, std::memory_order_relaxed);
    data[2].store(47, std::memory_order_relaxed);
    data[3].store(49, std::memory_order_relaxed);
    data[4].store(56, std::memory_order_relaxed);
    std::cout << "thread1" << std::endl;
    sync1.store(true, std::memory_order_release);
}

void thread_2_func()
{
    while (!sync1.load(std::memory_order_acquire));
    std::cout << "thread2" << std::endl;
    sync2.store(true, std::memory_order_release);
}

void thread_3_func()
{
    while (!sync2.load(std::memory_order_acquire));
    assert(data[0].load(std::memory_order_relaxed) == 42);
    assert(data[1].load(std::memory_order_relaxed) == 45);
    assert(data[2].load(std::memory_order_relaxed) == 47);
    assert(data[3].load(std::memory_order_relaxed) == 49);
    assert(data[4].load(std::memory_order_relaxed) == 56);
    std::cout << "thread3" << std::endl;
}

void run_transitive_sync()
{
    std::thread thread_1(thread_1_func);
    std::thread thread_2(thread_2_func);
    std::thread thread_3(thread_3_func);
    thread_1.join();
    thread_2.join();
    thread_3.join();
}

struct OPE {
    int i;
    std::string s;
};

std::atomic<OPE*> p;
std::atomic<int> a;

void create_ope()
{
    OPE* o = new OPE;
    o->i = 42;
    o->s = "hello";
    a.store(20, std::memory_order_relaxed);
    p.store(o, std::memory_order_release);
}

void use_ope()
{
    OPE* o;
    while (!(o = p.load(std::memory_order_consume)));
    assert(o->i == 42);
    assert(o->s == "hello");
    // not guaranty a equal 20
    assert(a.load(std::memory_order_relaxed) == 20);
    std::cout << o->s << std::endl;
}

void run_carries_a_dependency()
{
    std::thread thread_1(create_ope);
    std::thread thread_2(use_ope);
    thread_1.join();
    thread_2.join();
}

int main()
{
    run_flag();
    run_bool();
    run_compare_exchange();
    run_atomic_pointer();
    run_happen_before();
    run_mem_order();
    run_memory_order_seq_cst();
    run_memory_order_relaxed();
    run_memory_order_release();
    run_memory_order_release_acquire();
    run_transitive_sync();
    run_carries_a_dependency();
    return 0;
}
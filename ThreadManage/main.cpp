#include "common.h"
#include <iostream>
#include <thread>

void foo() { printf("Hello from foo \n"); }

class callable_class {
public:
    void operator()() { printf("Hello from class function operator\n"); }
};

// lambda syntax
/*
 * [capture-list] (params) mutable? constexpr? exception_attribute? -> ret {body}
 * [capture-list] (params) -> ret {body}
 * [capture-list] (params) {body}
 * [capture-list] {body}
 */

void run1()
{
    std::thread thread1(foo);
    callable_class obj;
    std::thread thread2(obj);
    std::thread thread3([] { printf("Hello from lambda\n"); });
    thread1.join();
    thread2.join();
    thread3.join();
    printf("Hello from main \n");
}

void funcC() { printf("Hello from funcC\n"); }
void funcA()
{
    printf("Hello from funcA \n");
    std::thread threadC(funcC);
    threadC.join();
}
void funcB() { printf("Hello from funcB \n"); }

void run2()
{
    std::thread threadA(funcA);
    std::thread threadB(funcB);
    threadA.join();
    threadB.join();
    printf("Hello from main \n");
}

void run3()
{
    std::thread thread1(funcB);

    // non parameter default is not joinable
    std::thread thread2;
    if (thread2.joinable()) {
        std::cout << "joinable" << std::endl;
    } else {
        std::cout << "non joinable" << std::endl;
    }
}

void foo4()
{
    std::this_thread::sleep_for(std::chrono::milliseconds(50));
    std::cout << "Hello from foo4" << std::endl;
}

void bar4()
{
    std::this_thread::sleep_for(std::chrono::milliseconds(50));
    std::cout << "Hello from bar4" << std::endl;
}

void other_operations()
{
    std::cout << "This is other operation \n";
    throw std::runtime_error("this is a runtime error");
}

void run4()
{
    std::thread foo_thread(foo4);
    std::thread bar_thread(bar4);

    std::cout << "before bar detach" << std::endl;
    bar_thread.detach();
    std::cout << "after bar detach" << std::endl;
    std::cout << "before foo join" << std::endl;
    foo_thread.join();
    std::cout << "after foo join" << std::endl;
}

void run5()
{
    std::thread foo_thread(foo);
    thread_guard tg(foo_thread);
    try {
        other_operations();
    } catch (...) {
    }
}

int main()
{
    run1();
    run2();
    // run3(); // std::terminated called
    run4();
    run5();
    return 0;
}
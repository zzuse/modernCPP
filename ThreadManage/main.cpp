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

int main()
{
    run1();
    run2();
    return 0;
}
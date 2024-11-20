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

// thread constructor
/*
 *   1. default: thread() noexcept
 *   2. init:  template <class Fn, class...Args>
 *             explicit thread(Fn&& fn, Args&&... args);
 *   3. copy[deleted]: thread(const thread&) = delete;
 *   4. move: thread(thread&& x) noexcept;
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

void func_1(int x, int y) { std::cout << x + y << std::endl; }

void func_2(int& x)
{
    int i = 0;
    while (i < 10) {
        std::cout << "Thread 1 value of X - " << x << std::endl;
        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
        i++;
    }
}

void run6()
{
    int x = 9;
    std::cout << "Main thread value of x - " << x << std::endl;
    // just need to mind the x will live out of the thread
    std::thread thread_1(func_2, std::ref(x));
    std::this_thread::sleep_for(std::chrono::milliseconds(5000));
    x = 15;
    std::cout << "Main thread has been change to - " << x << std::endl;
    thread_1.join();
}

void func_3(int& x)
{
    while (true) {
        try {
            std::cout << "thread 3 value of x - " << x << std::endl;
            std::this_thread::sleep_for(std::chrono::milliseconds(1000));
        } catch (...) {
            throw std::runtime_error("this is a runtime error");
        }
    }
}

void func_4()
{
    int x = 5;
    std::thread thread_3(func_3, std::ref(x));
    thread_3.detach();
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    std::cout << "thread_3 finished execution\n";
}

void run7()
{
    std::thread thread_1(func_4);
    thread_1.join();
}

enum class Command : std::uint16_t { INVALID_COMMAND, CLEAN = 1, FULL_SPEED = 2, STOP_ENGINE = 3, PROGRAM_EXIT = 100 };

void provideTheCommandCaptain()
{
    std::cout << "Captain please issue the command\n"
                 "1: Clean the ship\n"
                 "2: Full speed sailing\n"
                 "3: Stop the ship\n"
                 "100: End the program ship\n";
}

Command InttoCommand(std::uint16_t input)
{
    Command retVal = Command::INVALID_COMMAND;
    switch (static_cast<Command>(input)) {
        case Command::CLEAN:
            retVal = Command::CLEAN;
            break;
        case Command::FULL_SPEED:
            retVal = Command::FULL_SPEED;
            break;
        case Command::STOP_ENGINE:
            retVal = Command::STOP_ENGINE;
            break;
        case Command::PROGRAM_EXIT:
            retVal = Command::PROGRAM_EXIT;
            break;
        default:
            retVal = Command::INVALID_COMMAND;
            break;
    }
    return retVal;
}

bool issueCommand(const Command& command)
{
    switch (command) {
        case Command::CLEAN: {
            std::thread clean_ship([] {
                std::cout << "Cleaning the ship id" << std::this_thread::get_id() << "\n";
                std::this_thread::sleep_for(std::chrono::milliseconds(5000));
                std::cout << "Cleaning the ship done "
                          << "\n";
            });
            clean_ship.detach();
        }
            return true;
        case Command::FULL_SPEED: {
            std::thread full_speed([] {
                std::cout << "Ship on Full speed - id " << std::this_thread::get_id() << "\n";
                std::this_thread::sleep_for(std::chrono::milliseconds(5000));
                std::cout << " Ship changed the speed done "
                          << "\n";
            });
            full_speed.join();
        }
            return true;
        case Command::STOP_ENGINE: {
            std::thread stop_engine([] {
                std::cout << "Stop the Engine - id " << std::this_thread::get_id() << "\n";
                std::this_thread::sleep_for(std::chrono::milliseconds(1000));
                std::cout << " Stop the Engine done "
                          << "\n";
            });
            stop_engine.join();
        }
            return true;
        case Command::PROGRAM_EXIT:
            std::cout << "Main: Docking the ship\n";
            return false;
        case Command::INVALID_COMMAND:
        default:
            std::cout << "Invalid order\n";
            return true;
    };
}

int main()
{
    run1();
    run2();
    // run3(); // std::terminated called
    run4();
    run5();
    provideTheCommandCaptain();
    bool isSailing = true;
    while (isSailing) {
        std::cout << "Captian : Please give the command\n";
        std::uint16_t input;
        std::cin >> input;
        const auto& command = InttoCommand(input);
        isSailing = issueCommand(command);
    }
    run6();
    run7();
    return 0;
}
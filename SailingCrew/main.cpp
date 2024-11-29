#include "common.h"
#include <iostream>
#include <numeric>
#include <queue>
#include <thread>

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

void clean_ship()
{
    std::cout << "Cleaning the ship id" << std::this_thread::get_id() << "\n";
    std::this_thread::sleep_for(std::chrono::milliseconds(5000));
    std::cout << "Cleaning the ship done "
              << "\n";
}

void stop_engine()
{
    std::cout << "Stop the Engine - id " << std::this_thread::get_id() << "\n";
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    std::cout << " Stop the Engine done "
              << "\n";
}

void full_speed()
{
    std::cout << "Ship on Full speed - id " << std::this_thread::get_id() << "\n";
    std::this_thread::sleep_for(std::chrono::milliseconds(5000));
    std::cout << " Ship changed the speed done "
              << "\n";
}

void startCleaningCrew(std::queue<bool>& cq, bool& done)
{
    while (done) {
        if (cq.empty()) {
            std::cout << "CQ no work to do " << std::this_thread::get_id() << "\n";
            // no work sleep two seconds
            std::this_thread::sleep_for(std::chrono::milliseconds(2000));
        } else {
            // have work dispatcher for one second
            std::cout << "CQ work to do " << std::this_thread::get_id() << "\n";
            cq.pop();
            clean_ship();
            std::this_thread::sleep_for(std::chrono::milliseconds(1000));
        }
    }
}

void startEngineCrew(std::queue<bool>& eq, bool& done)
{
    while (done) {
        if (eq.empty()) {
            std::cout << "EQ no work to do " << std::this_thread::get_id() << "\n";
            // no work sleep two seconds
            std::this_thread::sleep_for(std::chrono::milliseconds(2000));
        } else {
            std::cout << "EQ work to do " << std::this_thread::get_id() << "\n";
            bool op = eq.front();
            eq.pop();
            if (op) {
                full_speed();
            } else {
                stop_engine();
            }
            // have work dispatcher for one second
            std::this_thread::sleep_for(std::chrono::milliseconds(1000));
        }
    }
}

void issueCommand(const Command& command, std::queue<bool>& cq, std::queue<bool>& eq, bool& done)
{
    switch (command) {
        case Command::CLEAN: {
            cq.push(true);
            break;
        }
        case Command::FULL_SPEED: {
            eq.push(true);
            break;
        }
        case Command::STOP_ENGINE: {
            eq.push(false);
            break;
        }
        case Command::PROGRAM_EXIT: {
            std::cout << "Main: Docking the ship\n";
            done = false;
            break;
        }
        case Command::INVALID_COMMAND:
        default:
            std::cout << "Invalid order\n";
            break;
    };
}

void runAccumulate()
{
    // T accumulate(InputIt first, InputIt last, T init);
    std::vector<int> v{1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
    std::cout << "sum - " << std::accumulate(v.begin(), v.end(), 0) << std::endl;
    // T accumulate(InputIt first, InputIt last, T init, BinaryOperation op);
    std::cout << "product - " << std::accumulate(v.begin(), v.end(), 1, std::multiplies<int>()) << std::endl;
    // lambda for the operator
    auto dash_fold = [](std::string a, int b) { return std::move(a) + "-" + std::to_string(b); };
    std::cout << "dash fold : " << std::accumulate(v.begin(), v.end(), std::to_string(v[0]), dash_fold) << std::endl;
}

#define MIN_BLOCK_SIZE 1000

template <typename iterator, typename T>
T accumulate(iterator start, iterator end, T& ref)
{
    ref = std::accumulate(start, end, 0);
    return ref;
}

template <typename iterator, typename T>
T parallel_accumulate(iterator start, iterator end, T& ref)
{
    int input_size = std::distance(start, end);
    int allowed_threads_by_elements = (input_size) / MIN_BLOCK_SIZE;
    int allowed_threads_by_hardware = std::thread::hardware_concurrency();
    int num_threads = std::min(allowed_threads_by_elements, allowed_threads_by_hardware);
    int block_size = (input_size + 1) / num_threads;

    std::vector<T> results(num_threads);
    std::vector<std::thread> threads(num_threads - 1);

    iterator last;
    for (int i = 0; i < num_threads - 1; i++) {
        last = start;
        std::advance(last, block_size);
        threads[i] = std::thread(accumulate<iterator, T>, start, last, std::ref(results[i]));
        start = last;
    }

    results[num_threads - 1] = std::accumulate(start, end, 0);
    std::for_each(threads.begin(), threads.end(), std::mem_fn(&std::thread::join));
    return std::accumulate(results.begin(), results.end(), ref);
}

void run_parallel_accumulate()
{
    const int size = 8000;
    int* my_array = new int[size];
    int ref = 0;
    srand(std::time(0));
    for (size_t i = 0; i < size; i++) {
        my_array[i] = rand() % 10;
    }
    int ref_val = parallel_accumulate<int*, int>(my_array, my_array + size, ref);
    std::cout << "Accumulated value: " << ref_val << std::endl;
}

int main()
{
    runAccumulate();
    run_parallel_accumulate();

    provideTheCommandCaptain();
    std::queue<bool> cq, eq;
    bool isSailing = true;
    std::thread cct(startCleaningCrew, std::ref(cq), std::ref(isSailing));
    std::thread ect(startEngineCrew, std::ref(eq), std::ref(isSailing));
    cct.detach();
    ect.detach();
    while (isSailing) {
        std::cout << "Captian : Please give the command\n";
        std::uint16_t input;
        std::cin >> input;
        const auto& command = InttoCommand(input);
        issueCommand(command, cq, eq, isSailing);
        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    }
    return 0;
}
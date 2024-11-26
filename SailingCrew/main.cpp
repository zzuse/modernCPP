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

int main()
{
    runAccumulate();

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
#include "common.h"
#include <iostream>
#include <list>
#include <mutex>
#include <stack>
#include <thread>
#include <chrono>
#include <queue>
#include <condition_variable>

bool have_i_arrived = false;
int distance_my_destination = 10;
int distance_coverd = 0;
std::condition_variable cv;
std::mutex m;

bool keep_driving() {
    while(true) {
        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
        distance_coverd++;
        // notify when events occurs
        if(distance_coverd == distance_my_destination)
            cv.notify_one();
        if (distance_coverd > 20) return true;
    }
    return false;
}

void keep_awake_all_night() {
    while (distance_coverd < distance_my_destination) {
        std::cout << "keep check, whether i am there \n";
        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    }
    std::cout << "finally i am there, awake all night, distance_coverd = " << distance_coverd << std::endl;
}

void ask_driver_to_wake_u_up_at_right_time(){
    std::unique_lock<std::mutex> ul(m);
    // need provide a lock and a condition to check after wake up
    // wake up can be due to notification from another thread or spurious wake
    // if condition is false, this thread will call unlock and goto sleep.
    // if condition is true, this thread will call lock and proceed until thread finish.
    // only unique_lock can provide that flexibility, so not lock_guard.
    cv.wait(ul, [] {return distance_coverd == distance_my_destination;});
    std::cout << "finally i am there, notify by driver, distance_coverd = " << distance_coverd << std::endl;
}

void set_the_alarm_and_take_a_nap() {
    if(distance_coverd < distance_my_destination) {
        std::cout << "let me take a nap\n";
        std::this_thread::sleep_for(std::chrono::milliseconds(10000));
    }
    std::cout << "finally i am there, take a nap, distance_coverd = " << distance_coverd << std::endl;
}

void run_code() {
    std::thread driver_thread(keep_driving);
    std::thread keep_awake_all_night_thread(keep_awake_all_night);
    std::thread set_the_alarm_and_take_a_nap_thread(set_the_alarm_and_take_a_nap);
    std::thread passenger_thread(ask_driver_to_wake_u_up_at_right_time);

    keep_awake_all_night_thread.join();
    set_the_alarm_and_take_a_nap_thread.join();
    driver_thread.join();
    passenger_thread.join();
}

template <typename T>
class thread_safe_queue {
    std::mutex m;
    std::condition_variable cv;
    // store pointer instead of raw value
    std::queue<std::shared_ptr<T>> queue;
public:
    thread_safe_queue();
    thread_safe_queue(thread_safe_queue const& other_queue);

    void push(T value) {
        std::lock_guard<std::mutex> lg(m);
        queue.push(std::make_shared<T>(value));
        cv.notify_one();
    }
    bool pop(T& ref);
    std::shared_ptr<T> pop() {
        std::lock_guard<std::mutex> lg(m);
        if (queue.empty())
        {
            return std::shared_ptr<T>();
        }
        else {
            std::shared_ptr<T> ref(queue.front());
            queue.pop();
            return ref;
        }
    }
    void wait_pop(T& ref);

    std::shared_ptr<T> wait_pop() {
        std::unique_lock<std::mutex> lg(m);
        cv.wait(lg, [this]{
            return !queue.empty();
        });
        std::shared_ptr<T> ref = queue.front();
        queue.pop();
        return ref;
    }

    bool empty(){
        std::lock_guard<std::mutex> lg(m);
        return queue.empty();
    }

    size_t size();
};

int main()
{
    run_code();
    return 0;
}
#include <chrono>
#include <future>
#include <iostream>
#include <list>
#include <mutex>
#include <string>
#include <thread>

std::list<int> g_Data;
const int SIZE = 500000;
std::mutex g_Mutex;

void Download(const std::string& file)
{
    std::cout << "Downloading..." << std::endl;
    for (int i = 0; i < SIZE; ++i) {
        g_Mutex.lock();
        g_Data.push_back(i);
        // if (i == 500) return; // dead lock will happen
        g_Mutex.unlock();
    }
    std::cout << "Download1 finished." << std::endl;
}

void Download2(const std::string& file)
{
    std::cout << "Downloading..." << std::endl;
    for (int i = 0; i < SIZE; ++i) {
        g_Mutex.lock();
        g_Data.push_back(i);
        g_Mutex.unlock();
    }
    std::cout << "Download2 finished." << std::endl;
}

int Add(int x, int y) { return x + y; }

int Square(int x) { return x * x; }

int Compute(const std::vector<int>& data)
{
    using namespace std::chrono_literals;
    int sum{};
    for (auto e : data) {
        sum += e;
        std::this_thread::sleep_for(1s);
        std::cout << '.';
    }
    return sum;
}

int main()
{
    std::string file("cppcast.mp4");
    std::cout << "User start download" << std::endl;
    std::thread thDownloader(Download, std::cref(file));
    std::thread thDownloader2(Download2, std::cref(file));
    // thDownloader.detach();
    std::cout << "User start another operation" << std::endl;
    if (thDownloader.joinable()) {
        thDownloader.join();
    }
    thDownloader2.join();
    std::cout << g_Data.size() << std::endl;

    std::packaged_task<int(int, int)> taskAdd{Add};
    std::future<int> ft = taskAdd.get_future();
    taskAdd(3, 5);
    auto result = ft.get();
    std::cout << result << std::endl;

    std::packaged_task<int(int)> taskSquare{Square};
    auto fSquare = taskSquare.get_future();
    taskSquare(5);
    std::cout << fSquare.get() << std::endl;

    std::packaged_task<int(const std::vector<int>&)> taskCompute{Compute};
    auto fCompute = taskCompute.get_future();
    std::vector<int> data{1, 2, 3, 4, 5};
    // taskCompute(data);
    std::thread threadCompute{std::move(taskCompute), data};
    std::cout << "Thread has started...\n";
    std::cout << fCompute.get() << std::endl;

    std::cout << "End of main()" << std::endl;
    threadCompute.join();
    return 0;
}
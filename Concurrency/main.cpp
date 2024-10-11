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

int Operation(int count)
{
    using namespace std::chrono_literals;
    int sum{};
    for (int i = 0; i < 10; ++i) {
        sum += i;
        std::cout << '.';
        std::this_thread::sleep_for(1s);
        std::cout.flush();
    }
    return sum;
}

void Download1(const std::string& file)
{
    std::cout << "Thread: " << std::this_thread::get_id() << " Downloading1 ..." << std::endl;
    for (int i = 0; i < SIZE; ++i) {
        std::lock_guard<std::mutex> mtx(g_Mutex);
        // g_Mutex.lock();
        g_Data.push_back(i);
        if (i == 500) return;
        // g_Mutex.unlock();
    }
    std::cout << "Download1 finished." << std::endl;
}

void Download2(const std::string& file)
{
    std::cout << "Thread: " << std::this_thread::get_id() << " Downloading2 ..." << std::endl;
    for (int i = 0; i < SIZE; ++i) {
        std::lock_guard<std::mutex> mtx(g_Mutex);
        // g_Mutex.lock();
        g_Data.push_back(i);
        // g_Mutex.unlock();
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
    std::thread thDownloader(Download1, std::cref(file));
    std::thread thDownloader2(Download2, std::cref(file));
    // thDownloader.detach();
    std::cout << "User start another operation" << std::endl;

    // Mac OS X: must be set from within the thread (can't specify thread ID)
    pthread_setname_np("Main");

    auto threadid = thDownloader.get_id();
    std::cout << "Downloader1 threadid: " << threadid << std::endl;
    char thread_name[8];
    pthread_t handle = thDownloader.native_handle();
    pthread_getname_np(handle, thread_name, 8);
    std::cout << "Downloader1 thread name:" << thread_name << std::endl;

    std::cout << "Cores:" << std::thread::hardware_concurrency() << std::endl;
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

    std::future<int> operation_result = std::async(Operation, 10);
    std::cout << "main() threading" << std::endl;
    std::cout << operation_result.get() << std::endl;
    std::cout << "main() finished" << std::endl;
    return 0;
}
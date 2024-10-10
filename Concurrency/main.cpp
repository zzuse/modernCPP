#include <iostream>
#include <list>
#include <string>
#include <thread>

std::list<int> g_Data;
const int SIZE = 500000;

void Download(const std::string& file)
{
    std::cout << "Downloading..." << std::endl;
    for (int i = 0; i < SIZE; ++i) {
        g_Data.push_back(i);
    }
    std::cout << "Downloading finished." << std::endl;
}

int main()
{
    std::string file("cppcast.mp4");
    std::cout << "User start download" << std::endl;
    std::thread thDownloader(Download, std::cref(file));
    // thDownloader.detach();
    std::cout << "User start another operation" << std::endl;
    if (thDownloader.joinable()) {
        thDownloader.join();
    }
    return 0;
}
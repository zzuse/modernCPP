#include <iostream>
#include <list>

std::list<int> g_Data;
const int SIZE = 500000;
void Download()
{
    std::cout << "Downloading..." << std::endl;
    for (int i = 0; i < SIZE; ++i) {
        g_Data.push_back(i);
    }
    std::cout << "Downloading finished." << std::endl;
}

int main()
{
    std::cout << "User start download" << std::endl;
    Download();
    std::cout << "User start another operation" << std::endl;
    return 0;
}
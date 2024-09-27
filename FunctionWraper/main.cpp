#include <fstream>
#include <iostream>
#include <string>
#include <vector>

int Square(int) { return 0; }

int main()
{
    std::function<int(int)> f1 = Square;
    std::function<bool(int)> f3 = [](int x) { return x % 2 == 0; };
    return 0;
}
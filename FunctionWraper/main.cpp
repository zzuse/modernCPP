#include <fstream>
#include <functional>
#include <iostream>
#include <string>
#include <vector>

int Square(int x) { return x * x; }

int Add(int x, int y) { return x + y; }

int Sub(int x, int y) { return x - y; }

struct Max {
    int operator()(int x, int y) const { return x > y ? x : y; }
};

// general purpose function wraper can accept any kind of callable,
// int Operation(int x, int y, Callback callback) not work with function objects and lambda functions
int Operation(int x, int y, std::function<int(int, int)> callback) { return callback(x, y); }

int main()
{
    std::function<int(int)> f1 = Square;
    std::cout << f1(3) << std::endl;

    try {
        std::function<int(int)> f2;
        if (f2) {
            std::cout << f2(3) << std::endl;
        } else {
            std::cout << "No target to invoke!\n";
        }

        std::function<bool(int)> f3 = [](int x) { return x % 2 == 0; };
        std::cout << std::boolalpha << "Is Even? " << f3(4) << std::endl;

        Max m{};
        std::function<int(int, int)> f4 = m;
        std::cout << f4(3, 5) << std::endl;

        std::function<int(int, int)> f5 = Add;
        std::cout << Operation(3, 5, f5) << std::endl;
        f5 = Sub;
        std::cout << Operation(3, 5, f5) << std::endl;
        std::cout << Operation(3, 5, [](int x, int y) { return x * y; }) << std::endl;

    } catch (const std::exception& ex) {
        std::cout << "Exception -> " << ex.what() << std::endl;
    }
    return 0;
}
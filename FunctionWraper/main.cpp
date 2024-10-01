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

struct GreaterThan {
    bool operator()(int x, int y) const { return x > y; }
};

// general purpose function wraper can accept any kind of callable,
// int Operation(int x, int y, Callback callback) not work with function objects and lambda functions
int Operation(int x, int y, std::function<int(int, int)> callback) { return callback(x, y); }

int Accumulate(int x, int y, int z) { return x + y + z; }

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

    // bind specific parameter
    auto f6 = std::bind(Square, 3);
    std::cout << "f6 -> " << f6() << std::endl;

    // using std::bind can change parameter's position
    std::function<int(int, int)> f7 = std::bind(Sub, std::placeholders::_2, std::placeholders::_1);
    std::cout << "f7(3,2) -> " << f7(3, 2) << std::endl;

    // simplified version of f7
    using namespace std::placeholders;
    auto f8 = std::bind(Sub, _2, _1);
    std::cout << "f8(3,2) -> " << f8(3, 2) << std::endl;

    // std::bind can implement fixed parameter
    // Lambda version of bind is easier, but std::bind will forward r value default
    auto f9 = std::bind(Sub, _1, 5);
    std::cout << "f9(10) -> " << f9(10) << std::endl;
    auto f91 = [](int x) { return Sub(x, 5); };
    std::cout << "f91(10) -> " << f91(10) << std::endl;
    // Lambda need special work to forward r value
    auto f92 = [](auto&& x) { return Sub(std::forward<decltype(x)>(x), 5); };
    std::cout << "f92(10) -> " << f92(10) << std::endl;

    auto f10 = std::bind(GreaterThan{}, _2, _1);
    std::cout << std::boolalpha << f10(6, 2) << std::endl;
    auto f101 = [](int x, int y) { return GreaterThan{}(y, x); };
    std::cout << std::boolalpha << f101(6, 2) << std::endl;
    // template lambda (C++20) r value forward
    auto f102 = []<typename T>(T&& x, T&& y) { return GreaterThan{}(std::forward<T>(y), std::forward<T>(x)); };
    std::cout << std::boolalpha << f102(6, 2) << std::endl;

    // 3 parameter to 2 parameter
    auto f11 = std::bind(Accumulate, _1, _2, 0);
    std::cout << "f11(8, 5) -> " << Operation(8, 5, f11) << std::endl;
    auto f111 = [](int x, int y) { return Accumulate(x, y, 0); };
    std::cout << "f111(8, 5) -> " << Operation(8, 5, f111) << std::endl;
    // template lambda (C++20) r value forward
    auto f112 = []<typename T>(T&& x, T&& y) { return Accumulate(std::forward<T>(x), std::forward<T>(y), 0); };
    std::cout << "f112(8, 5) -> " << Operation(8, 5, f112) << std::endl;
    return 0;
}
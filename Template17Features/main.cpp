#include <iostream>
#include <mutex>
#include <vector>

template <typename T>
class Data {
public:
    Data(const T& t) {}
};
Data(const char*)->Data<std::string>;
Data(int)->Data<long>;

// implement uppack using overloaded variadic templates and recursion
auto Sum() { return 0; }
template <typename T, typename... Args>
auto Sum(T a, Args... args)
{
    return a + Sum(args...);
}
/*
 * Sum(1,2,3,4,5)
 * return 1 + Sum(2,3,4,5)
 * return 2 + Sum(3,4,5)
 * return 3 + Sum(4,5)
 * return 4 + Sum(5)
 * return 5 + Sum();
 * return 5 + 0;
 *
 *  1+(2+(3+(4+(5+0))))
 *  1+(2+(3+(4+5)))
 *  1+(2+(3+9))
 *  1+(2+12)
 *  1+14
 *  15
 *
 */

template <typename... Args>
auto Sum2(Args... args)
{
    // return (args + ...); // Unary right fold (pack op ...)
    return (... + args); // Unary left fold (... op pack)
}
/*
 * Unary Right Fold
 * sum2(1,2,3,4,5)
 * (1+(2+(3+(4+5))))
 *
 * Unary Left Fold
 * ((((1+2)+3)+4)+5)
 */

int main()
{
    std::pair<int, int> p1{2, 5};
    auto p2 = std::make_pair(2, 5);

    std::pair p3{2, 5};
    std::vector v1{1, 2, 3, 4};

    std::mutex m;
    std::lock_guard lck{m};

    Data d1{5};
    Data d2{8.2f};
    Data d3 = d1;
    auto d4 = new Data{"Hello"};

    // if I want to deduct to string not char?
    Data d5{"Hello"};

    auto result = Sum(1, 2, 3, 4, 5);
    std::cout << result << std::endl;

    result = Sum2(1, 2, 3, 4, 5);
    std::cout << result << std::endl;
    return 0;
}
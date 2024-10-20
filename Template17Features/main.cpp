#include <iostream>
#include <mutex>
#include <type_traits>
#include <vector>

template <typename T>
class Data {
public:
    Data(const T& t) {}
};
Data(const char*) -> Data<std::string>;
Data(int) -> Data<long>;

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

template <typename... Args>
auto Sum3(Args... args)
{
    return (0 + ... + args); // Binary Left Fold
    // return (args + ... + 0); // Binary Right Fold
}
/*
 * Binary Left Fold
 * (((((0+1)+2)+3)+4)+5)
 * Binary Right Fold
 * (1+(2+(3+(4+(5+0))))
 */

/* operators can be used with fold expressions
 + - * / %^& | = < > << >> += -= *= /= %= ^= &= |= <<= >>= == != <= >= && || , .* ->*
*/

/* empty pack of the value:
 * &&    - true
 * ||    - false
 * ,     - void
 * other - ill formed
 */

// any of the list is even number
template <typename... Args>
bool AnyOf(Args... args)
{
    return (... || (args % 2 == 0));
}
// all of the list is even number
template <typename... Args>
bool AllOf(Args... args)
{
    return (... && (args % 2 == 0));
}
// use Predict to check expression return true or false
template <typename... Args, typename Predicate>
bool AnyOf2(Predicate p, Args... args)
{
    return (... || p(args));
}

// type traits
template <typename T>
T Divide(T a, T b)
{
    if (std::is_floating_point_v<T> == false) {
        std::cout << "Use floating point types only\n";
        return 0;
    }
    return a / b;
}
template <typename T>
void Check(T&&)
{
    std::cout << std::boolalpha;
    std::cout << "Is reference?" << std::is_reference_v<T> << std::endl;
    std::cout << "After removing:" << std::is_reference<typename std::remove_reference<T>::type>::value << std::endl;
    std::cout << "After removing:" << std::is_reference_v<typename std::remove_reference_t<T>> << std::endl;
}

class Number {
    int m_Number;

public:
    Number() = default;
    Number(int x)
        : m_Number{x}
    {
    }
};

template <typename T>
void Print(const T& value)
{
    if constexpr (std::is_pointer_v<T>) {
        std::cout << *value << std::endl;
    } else if constexpr (std::is_array_v<T>) {
        for (auto v : value) {
            std::cout << v << ' ';
        }
    } else {
        std::cout << value << std::endl;
    }
}

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

    result = Sum3(1, 2, 3, 4, 5);
    std::cout << result << std::endl;

    std::cout << std::boolalpha;
    std::cout << "Any Even? " << AnyOf(10, 3, 5) << std::endl;
    std::cout << "All Even? " << AllOf(10, 3, 5) << std::endl;
    std::cout << "Any Even? " << AnyOf2([](int x) { return x % 2 == 0; }, 10, 3, 5) << std::endl;

    static_assert(std::is_default_constructible<Number>::value,
                  "Only objects with default constructor can be deserialized.");
    static_assert(std::is_default_constructible_v<Number>,
                  "Only objects with default "
                  "constructor can be deserialized.");

    Divide(5, 6);
    Check(result);

    int value{5};
    Print(value);
    Print(&value);
    int arr[] = {1, 2, 3, 4, 5};
    Print(arr);
    return 0;
}
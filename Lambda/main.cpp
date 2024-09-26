#include <fstream>
#include <iostream>
#include <string>
#include <vector>

// using Comparator = bool (*)(int, int);
template <typename T, int size, typename Comparator>
void Sort(T (&arr)[size], Comparator comp)
{
    for (int i = 0; i < size - 1; ++i) {
        for (int j = 0; j < size - 1; ++j) {
            if (comp(arr[j], arr[j + 1])) {
                T temp = std::move(arr[j]);
                arr[j] = std::move(arr[j + 1]);
                arr[j + 1] = std::move(temp);
            }
        }
    }
}

bool Comp(int x, int y) { return x > y; }
bool Comp1(int x, int y) { return x < y; }
struct Comp2 {
    bool operator()(int x, int y) { return x > y; }
};

template <typename T, int size, typename Callback>
void ForEach(T (&arr)[size], Callback operation)
{
    for (int i = 0; i < size; i++) {
        operation(arr[i]);
    }
}

class Product {
    std::string name;
    float price;

public:
    Product(const std::string &n, float p)
        : name(n)
        , price(p)
    {
    }
    void AssignFinalPrice()
    {
        float taxes[]{12, 5, 6};
        float basePrice{price};
        ForEach(taxes, [basePrice, this](float tax) {
            float taxedPrice = basePrice * tax / 100;
            price += taxedPrice;
        });
    }
    float GetPrice() const { return price; }
};

// lambda expression & equivalent function object
auto Max1 = [](auto x, auto y) {
    if (x > y) {
        return x;
    } else {
        return y;
    }
};
struct Max2 {
    template <typename T>
    T operator()(T x, T y)
    {
        if (x > y) {
            return x;
        } else {
            return y;
        }
    }
};

auto Greater1 = [](auto x, auto y) {
    if (x > y) {
        return true;
    } else
        return false;
};
struct Greater2 {
    template <typename T>
    bool operator()(T x, T y)
    {
        if (x > y) {
            return true;
        } else
            return false;
    }
};

auto MinMax1 = [](auto iter_begin, auto iter_end) {
    auto min{*iter_begin};
    auto max{*iter_begin};
    while (iter_begin != iter_end) {
        auto element = *iter_begin;
        if (element > max) {
            max = element;
        }
        if (element < min) {
            min = element;
        }
        iter_begin++;
    }
    return std::make_pair(min, max);
};

template <typename T>
struct MinMax2 {
    template <typename I>
    auto operator()(I iter_begin, I iter_end)
    {
        typename I::value_type min{*iter_begin};
        typename I::value_type max{*iter_begin};
        while (iter_begin != iter_end) {
            auto element = *iter_begin;
            if (element > max) {
                max = element;
            }
            if (element < min) {
                min = element;
            }
            iter_begin++;
        }
        return std::pair<T, T>(min, max);
    }
};

int main()
{
    Comp(3, 5);
    Comp2 comp;
    comp(3, 5); // syntactic sugar of comp.operator()(3,5);

    int arr[]{1, 6, 8, 4, 0};
    for (auto x : arr) {
        std::cout << x << " ";
    }
    std::cout << std::endl;
    Sort(arr, comp);
    for (auto x : arr) {
        std::cout << x << " ";
    }
    std::cout << std::endl;
    auto fn = []() { std::cout << "Welcome to Lambda expressions" << std::endl; };
    fn();
    std::cout << typeid(fn).name() << std::endl;
    // generic lambda
    auto sum = [](auto x, auto y) noexcept(false) { return x + y; };
    std::cout << "Sum is: " << sum(5.5f, 2.2f) << std::endl;

    // a list add offset each, copy captured variable
    ForEach(arr, [](auto x) { std::cout << x << " "; });
    std::cout << std::endl;
    int offset = 5;
    ForEach(arr, [offset](auto &x) { x += offset; });
    ForEach(arr, [](auto x) { std::cout << x << " "; });
    std::cout << std::endl;
    // a list add offset + 1, modify captured variable
    ForEach(arr, [&offset](auto &x) {
        x += offset;
        offset++;
    });
    ForEach(arr, [](auto x) { std::cout << x << " "; });
    std::cout << std::endl;

    // capture offset by value, and other by reference
    int all{};
    ForEach(arr, [&, offset](auto &x) { all += x; });
    // capture all by reference, and other by value
    ForEach(arr, [=, &all](auto &x) { all += x; });
    std::cout << "all: " << all << std::endl;

    // capture member valriable
    Product p{"Apple Watch", 569};
    p.AssignFinalPrice();
    std::cout << p.GetPrice() << std::endl;

    // generalized lambda
    // capture only used inside of lambda
    std::ofstream out{"file.txt"};
    auto write = [out = std::move(out)](int x) mutable { out << x; };
    write(200);

    Max2 max2;
    std::cout << Max1(1, 2) << " " << max2(1, 2) << std::endl;

    Greater2 greater2;
    std::cout << std::boolalpha << Greater1(2.1, 2.3) << " " << greater2(2.1, 2.3) << std::endl;

    std::vector<int> v1{1, 2, 3, 4, 5};
    auto minmax = MinMax1(v1.begin(), v1.end());
    std::cout << minmax.first << " " << minmax.second << std::endl;
    MinMax2<int> minmax2;
    auto minmax1 = minmax2(v1.begin(), v1.end());
    std::cout << minmax1.first << " " << minmax1.second << std::endl;

    return 0;
}
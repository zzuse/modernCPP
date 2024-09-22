#include <iostream>
#include <string>

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
    return 0;
}
#include <filesystem>
#include <fstream>
#include <iostream>
#include <string>

// return sum list
template <typename T>
T Sum(T* arr, int size)
{
    T sum{};
    for (int i = 0; i < size; ++i) {
        sum += arr[i];
    }
    return sum;
}

// return max in list
template <typename T>
T Max(T* arr, int size)
{
    T max{};
    for (int i = 0; i < size; ++i) {
        if (arr[i] > max) {
            max = arr[i];
        }
    }
    return max;
}

int main()
{
    float a[] = {1.1, 2.2, 3.3, 4.4};
    std::cout << Sum(a, 4) << std::endl;
    std::cout << Max(a, 4) << std::endl;

    return 0;
}
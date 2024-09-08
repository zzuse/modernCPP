#include "Integer.h"
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

template <typename T>
T Max(T x, T y)
{
    std::cout << typeid(T).name() << std::endl;
    return x > y ? x : y;
}

// Explicit Instantiation
template char Max(char x, char y);

// Explicit Specialization
template <>
const char* Max<const char*>(const char* x, const char* y)
{
    std::cout << "Max<const char*>()" << std::endl;
    return strcmp(x, y) > 0 ? x : y;
}

// Non Type Template
template <int size>
void Print()
{
    std::cout << size << std::endl;
}

template <typename T, int size>
T Sum_ref(T (&parr)[size])
{
    T sum{};
    for (int i = 0; i < size; ++i) {
        sum += parr[i];
    }
    return sum;
}

class Employee {
    std::string m_Name;
    Integer m_Id;

public:
    // Employee(const std::string& name, const Integer& id)
    //     : m_Name(name)
    //     , m_Id(id)
    // {
    //     std::cout << "Employee(const std::string &name, const Integer &id)" << std::endl;
    // }
    template <typename T1, typename T2>
    Employee(T1&& name, T2&& id)
        : m_Name(name)
        , m_Id(id)
    {
        std::cout << "Employee(const std::string &&name, const Integer &&id)" << std::endl;
    }
};

int main()
{
    float a[] = {1.1, 2.2, 3.3, 4.4};
    // can be a reference
    // float(&ref)[4] = a;

    std::cout << Sum(a, 4) << std::endl;
    std::cout << Sum_ref(a) << std::endl;
    std::cout << Max(a, 4) << std::endl;

    Max(static_cast<float>(3), 4.4f);
    int (*p)(int, int) = Max;
    p(2, 3);

    int i = 3;
    Print<sizeof(i)>();

    Employee emp{"Zhen", 100};
    return 0;
}
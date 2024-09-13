#include "Integer.h"
#include <filesystem>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

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
    template <typename T1, typename T2>
    Employee(T1&& name, T2&& id)
        : m_Name{std::forward<T1>(name)}
        , m_Id{std::forward<T2>(id)}
    {
        std::cout << "Employee(const std::string &&name, const Integer &&id)" << std::endl;
    }
};

template <typename T1, typename T2>
Employee* Create(T1&& a, T2&& b)
{
    return new Employee(std::forward<T1>(a), std::forward<T2>(b));
}

void Prints() { std::cout << std::endl; }

template <typename T, typename... Params>
void Prints(T&& a, Params&&... args)
{
    // std::cout << sizeof...(args) << std::endl;
    // std::cout << sizeof...(Params) << std::endl;
    std::cout << a;
    if (sizeof...(args) != 0) {
        std::cout << ",";
    }
    Prints(std::forward<Params>(args)...);
}

// Factory Create Objects
template <typename T, typename... Args>
T* CreateFactory(Args&&... args)
{
    return new T{std::forward<Args>(args)...};
}

// Stack Class Template
template <typename T, int size>
class Stack {
    T m_Buffer[size];
    int m_Top{-1};

public:
    void Push(const T& elem) { m_Buffer[++m_Top] = elem; }
    void Pop() { --m_Top; }
    const T& Top() const { return m_Buffer[m_Top]; }
    bool IsEmpty() { return m_Top == -1; }
    static Stack Create();
    Stack() = default;
    Stack(const Stack& obj)
    {
        m_Top = obj.m_Top;
        for (int i = 0; i <= m_Top; ++i) {
            m_Buffer[i] = obj.m_Buffer[i];
        }
    }
};

// Outside Class Template Declaration
template <typename T, int size>
Stack<T, size> Stack<T, size>::Create()
{
    return Stack<T, size>();
}

/// @brief Pretty Printer
/// @tparam T
template <typename T>
class PrettyPrinter {
    T* m_pData;

public:
    PrettyPrinter(T* data)
        : m_pData(data)
    {
    }

    void Print() { std::cout << "{" << *m_pData << "}" << std::endl; }

    T* GetData() { return m_pData; }
};

// Explicit Specialization for Class
template <>
class PrettyPrinter<char*> {
    char* m_pData;

public:
    PrettyPrinter(char* data)
        : m_pData(data)
    {
    }

    void Print() { std::cout << "{" << m_pData << "}" << std::endl; }

    char* GetData() { return m_pData; }
};

// Explicit Partial Specialization
template <>
void PrettyPrinter<std::vector<int>>::Print()
{
    std::cout << "{";
    for (auto x : *m_pData) {
        std::cout << x;
    }
    std::cout << "}" << std::endl;
}

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

    Employee emp{"Zhen", Integer{100}};
    Integer val{100};
    Employee emp2{std::string{"Zhen"}, val};
    auto emp3 = Create("Zhen", Integer{100});

    Prints(1, 2, 2.5, 3, "4", Integer{20});

    auto q = CreateFactory<Integer>(1);
    auto r = CreateFactory<Integer>();
    auto t = CreateFactory<Employee>("Zhen", Integer{100});

    Stack<float, 4> s = Stack<float, 4>::Create();
    s.Push(3.1);
    s.Push(1.1);
    s.Push(6.1);
    s.Push(9.1);
    while (!s.IsEmpty()) {
        std::cout << s.Top() << " ";
        s.Pop();
    }

    int data_1 = 5;
    float data_2 = 8.2f;
    PrettyPrinter<int> p1(&data_1);
    p1.Print();
    PrettyPrinter<float> p2(&data_2);
    p2.Print();
    char* data_3{"Hello World"};
    PrettyPrinter<char*> p3(data_3);
    p3.Print();
    char* pData = p3.GetData();
    std::cout << pData << std::endl;
    std::vector<int> data_4{1, 2, 3, 4, 5};
    PrettyPrinter<std::vector<int>> p4(&data_4);
    p4.Print();
    return 0;
}
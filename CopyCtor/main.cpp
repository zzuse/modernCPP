#include "Integer.h"
#include <cassert>
#include <initializer_list>
#include <iostream>
#include <memory>
#include <sstream>
#include <string>

enum class Case { SENSITIVE, INSENSITIVE };

class IntPtr {
    Integer *m_p;

public:
    IntPtr(Integer *p)
        : m_p(p)
    {
    }

    ~IntPtr() { delete m_p; }
    // RAII like a pointer
    Integer *operator->() { return m_p; }
    // RAII dereference a pointer
    Integer &operator*() { return *m_p; }
};

Integer Add(const Integer &a, const Integer &b)
{
    Integer temp;
    temp.SetValue(a.GetValue() + b.GetValue());
    return temp;
}

class Number {
    Integer m_Value{};

public:
    Number() = default;
    Number(int value)
        : m_Value{value}
    {
    }
    Number(const Number &n)
        : m_Value(n.m_Value)
    {
    }
    //    ~Number(){}
    Number(Number &&n)
        : m_Value{std::move(n.m_Value)}
    {
    }
    Number &operator=(Number &&) = default;
    Number &operator=(const Number &) = default;
};

class Bag {
    int arr[10];
    int m_Size{};

public:
    Bag(std::initializer_list<int> values)
    {
        assert(values.size() < 10);
        auto it = values.begin();
        while (it != values.end()) {
            Add(*it);
            ++it;
        }
    }
    void Add(int value)
    {
        assert(m_Size < 10);
        arr[m_Size++] = value;
    }
    void Remove() { --m_Size; }
    int operator[](int index) { return arr[index]; }
    int GetSize() const { return m_Size; }
};

Number CreateNumber(int num)
{
    Number n{num};
    return n;
}

void Process1(std::unique_ptr<Integer> ptr) { std::cout << ptr->GetValue() << std::endl; }
void Process2(std::shared_ptr<Integer> ptr) { std::cout << ptr->GetValue() << std::endl; }

void CreateInteger()
{
    IntPtr p = new Integer;
    p->SetValue(3);
    (*p).SetValue(4);
    std::cout << "IntPtr like a smart pointer " << p->GetValue() << std::endl;

    std::unique_ptr<Integer> p1(new Integer);
    p1->SetValue(3);
    Process1(std::move(p1));

    std::shared_ptr<Integer> p2(new Integer);
    p2->SetValue(5);
    Process2(p2);
}

class Product {
    Integer m_Id;

public:
    Product(const Integer &id)
        : m_Id{id}
    {
        std::cout << "Product(const Integer &)" << std::endl;
    }
    ~Product() { std::cout << "~"; }
    const Integer &GetInteger() const { return m_Id; }
    operator Integer() { return m_Id; }
};

class Distance {
    long double m_Kilometers;

public:
    Distance(long double km)
        : m_Kilometers{km}
    {
    }
    long double GetKm() const { return m_Kilometers; }
    void SetKm(long double val) { m_Kilometers = val; }
};

Distance operator"" _mi(long double val) { return Distance{val * 1.6}; }

void usingStdString();
size_t Find(const std::string &source, const std::string &search_string, Case searchCase, size_t offset = 0);

void Print(std::initializer_list<int> values)
{
    for (auto x : values) {
        std::cout << x << " ";
    }
}

int main()
{
    // Move Semantics
    Integer a(1), b(3);
    a.SetValue(Add(a, b).GetValue());
    // Move Semantics
    Number n1{1};
    auto n2{n1};
    n2 = n1;
    std::cout << "Hello, World!" << std::endl;
    auto n3{CreateNumber(3)};
    n3 = CreateNumber(3);

    // Smart Pointer
    CreateInteger();

    // Type Conversions: primitive type to primitive type
    int a1 = 5, b1 = 2;
    float f = static_cast<float>(a1) / b1;
    std::cout << "static_cast check valid or not: " << f << std::endl;
    char *p = reinterpret_cast<char *>(&a1);
    std::cout << "reinterpret_cast allow different type cast" << *p << std::endl;
    const int x = 1;
    int *q = const_cast<int *>(&x);
    std::cout << "const_cast allow const type cast: " << *q << std::endl;
    // Type Conversions: primitive type to user type
    Integer a2{5}; // init
    a2 = 7;        // assignment -- explicit keyword will destroy that
    // Type Conversions: user type to primitive type
    int x1 = static_cast<int>(a2); // -- explicit keyword prevent: int x1 = a2
    // Type Conversions: user type to user type
    Product p1{5};
    Integer id = p1; // syntax sugar: id = p1.operator Integer();
    if (id == p1.GetInteger()) {
        std::cout << "Id matches with Product: " << id.GetValue() << std::endl;
    }
    if (id == p1) {
        std::cout << "Id matches with Product: " << id.GetValue() << std::endl;
    }

    //    // Example 8
    //    usingStdString();

    //    // Example 9
    //    std::stringstream ss;
    //    std::string data= "12 89 21";
    //    ss.str(data);
    //    int a;
    //    while(ss >> a) {
    //        std::cout << a << std::endl;
    //    }
    //    std::cout << Find("abcd", "CD",  Case::INSENSITIVE) << std::endl;

    //    // Example 10
    //    Distance dist{32.0_mi};
    //    std::cout << dist.GetKm() << std::endl;

    //    // Example 11
    Bag bag{3, 1, 8};
    for (int i = 0; i < bag.GetSize(); ++i) {
        std::cout << bag[i] << " ";
    }
    Print({8, 2, 6, 7});
    return 0;
}

void usingStdString()
{
    std::string s = "Hello ";
    //    std::getline(std::cin, s);
    s.length();
    std::cout << s.length() << std::endl;

    s.insert(6, "World");
    std::cout << s << std::endl;

    if (s == "Hello World") {
        std::cout << s << std::endl;
    }

    s.erase(0, 6);
    std::cout << s << std::endl;

    using namespace std::string_literals;
    auto n2 = "Zen"s;
}

std::string ToLower(const std::string &str)
{
    std::string returnValue;
    for (const auto &c : str) {
        returnValue += std::tolower(c);
    }
    return returnValue;
}

size_t Find(const std::string &source, const std::string &search_string, Case searchCase = Case::INSENSITIVE,
            size_t offset)
{
    if (searchCase == Case::SENSITIVE) {
        return source.find(search_string, offset);
    } else {
        std::string source_lower = ToLower(source);
        std::string search_string_upper = ToLower(search_string);
        return source_lower.find(search_string_upper, offset);
    }
}
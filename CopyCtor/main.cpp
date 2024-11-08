#include "Integer.h"
#include <iostream>
#include <memory>
#include <sstream>
#include <string>

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

    return 0;
}

#include "Integer.h"
#include <iostream>
#include <memory>

class IntPtr {
    Integer *m_p;

public:
    IntPtr(Integer *p)
        : m_p(p)
    {
    }

    ~IntPtr() { delete m_p; }

    Integer *operator->() { return m_p; }

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

void CreateInteger()
{
    IntPtr p = new Integer;
    p->SetValue(3);
    (*p).SetValue(4);
    std::cout << p->GetValue() << std::endl;
}

class Product {
    Integer m_Id;
public:
    Product(const Integer &id):m_Id{id} {
        std::cout << "Product(const Integer &)" << std::endl;
    }
    ~Product() {
        std::cout << "~";
    }
    const Integer & GetInteger()const {
        return m_Id;
    }
    operator Integer() {
        return m_Id;
    }
};

void Display(Integer *p) {
    if(!p) {
        return;
    }
    std::cout << p->GetValue() << std::endl;
}

Integer* GetPointer(int value) {
    Integer* p = new Integer{value};
    return p;
}

void Operate(int value) {
    Integer* p = GetPointer(value);
    if (p == nullptr) {
        p = new Integer{value};
    }
    p->SetValue(100);
    Display(p);
    delete p;
    p = nullptr;
    p = new Integer{};
    *p = __LINE__;
    Display(p);
    delete p;
}

int main()
{
//    // Example 1
//    Integer a(1), b(3);
//    a.SetValue(Add(a,b).GetValue());
//    // Example 2
//    Number n1{1};
//    auto n2{n1};
//    n2 = n1;
//    std::cout << "Hello, World!" << std::endl;
//
//    auto n3{CreateNumber(3)};
//    n3 = CreateNumber(3);
//
//    // Example 3
//    CreateInteger();
//
//    // Example 4
//    int a = 5, b = 2;
//    float f = static_cast<float>(a) / b;
//    char *p = reinterpret_cast<char *>(&a);
//    const int x = 1;
//    int *q = const_cast<int *>(&x);
//
//    // Example 5
//    // user type to primitive type
//    Integer a1{5};
//    a1 = 7;
//    int x = static_cast<int>(a1);

//    // Example 6
//    Product p{5};
//    Integer id{5};
//    if (id == p.GetInteger()) {
//        std::cout << id.GetValue() << std::endl;
//    }
    Operate(5);
    return 0;
}

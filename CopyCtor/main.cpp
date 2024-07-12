#include "Integer.h"
#include <iostream>

class IntPtr {
    Integer *m_p;
public:
    IntPtr(Integer *p): m_p(p) {

    }

    ~IntPtr() {
        delete m_p;
    }

    Integer *operator ->() {
        return m_p;
    }

    Integer & operator *() {
        return *m_p;
    }
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

void CreateInteger() {
    IntPtr p = new Integer;
    p->SetValue(3);
    (*p).SetValue(4);
    std::cout << p->GetValue() << std::endl;
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

//    // Example 3
//    CreateInteger();
    int a = 5, b =2;
    float f = static_cast<float>(a)/b;
    char* p = reinterpret_cast<char*>(&a);
    const int x =1;
    int *q = const_cast<int*> (&x);
    std::cout << f << std::endl;
    return 0;
}

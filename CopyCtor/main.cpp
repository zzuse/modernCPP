#include "Integer.h"
#include <iostream>

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

int main()
{
    //    Example 1
    //    Integer a(1), b(3);
    //    a.SetValue(Add(a,b).GetValue());
    Number n1{1};
    auto n2{n1};
    n2 = n1;
    std::cout << "Hello, World!" << std::endl;

    auto n3{CreateNumber(3)};
    n3 = CreateNumber(3);

    return 0;
}

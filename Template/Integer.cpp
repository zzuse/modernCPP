//
// Created by 张臻 on 2024-06-24.
//
#include "Integer.h"
#include <iostream>
Integer::Integer()
{
    std::cout << "Integer()" << std::endl;
    m_pInt = new int(0);
}

Integer::Integer(int value)
{
    std::cout << "Integer(int)" << std::endl;
    m_pInt = new int(value);
}

Integer::Integer(const Integer& obj)
{
    std::cout << "Integer(const Integer&)" << std::endl;
    m_pInt = new int(*obj.m_pInt);
}

Integer::Integer(Integer&& obj)
{
    std::cout << "Integer(int&&)" << std::endl;
    m_pInt = obj.m_pInt;
    obj.m_pInt = nullptr;
}

Integer& Integer::operator=(const Integer& obj)
{
    std::cout << "operator=(const Integer& ojb)" << std::endl;
    if (this == &obj) {
        return *this;
    }
    delete m_pInt;
    m_pInt = new int(*obj.m_pInt);
    return *this;
}

Integer& Integer::operator=(Integer&& obj)
{
    std::cout << "operator=(Integer&& obj)" << std::endl;
    if (this == &obj) {
        return *this;
    }
    delete m_pInt;
    m_pInt = obj.m_pInt;
    obj.m_pInt = nullptr;
    return *this;
}

bool Integer::operator==(const Integer& a) const { return *m_pInt == *a.m_pInt; }

std::ostream& operator<<(std::ostream& out, const Integer& a)
{
    out << a.GetValue();
    return out;
}

int Integer::GetValue() const { return *m_pInt; }

void Integer::SetValue(int value) { *m_pInt = value; }

Integer::operator int() { return *m_pInt; }

Integer::~Integer()
{
    std::cout << "~Integer()" << std::endl;
    delete m_pInt;
}

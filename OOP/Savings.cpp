#include "Savings.h"
#include <iostream>

Savings::Savings(const std::string &name, float balance, float rate)
    : Account(name, balance)
    , m_Rate(rate)
{
    std::cout << "Savings(const std::string&, float, float)" << GetName() << std::endl;
}

Savings::~Savings() { std::cout << "~Savings()" << GetName() << std::endl; }

float Savings::GetInterestRate() const
{
    std::cout << __PRETTY_FUNCTION__ << " " << m_Rate << std::endl;
    return m_Rate;
}

void Savings::AccumulateInterest()
{
    std::cout << __PRETTY_FUNCTION__ << std::endl;
    m_Balance += m_Balance * m_Rate;
}

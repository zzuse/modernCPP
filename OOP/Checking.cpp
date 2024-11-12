#include "Checking.h"
#include <iostream>

Checking::Checking(const std::string &name, float balance, float minBalance)
    : Account(name, balance)
    , m_MinimumBalance(minBalance)
{
    std::cout << __PRETTY_FUNCTION__ << std::endl;
}

Checking::~Checking() { std::cout << "~Checking()" << GetName() << std::endl; }

void Checking::Withdraw(float amount)
{
    std::cout << __PRETTY_FUNCTION__ << std::endl;
    if (m_Balance - amount >= m_MinimumBalance) {
        Account::Withdraw(amount);
    } else {
        std::cout << "Insufficient balance" << std::endl;
    }
}

float Checking::GetMinimumBalance() const { return m_MinimumBalance; }

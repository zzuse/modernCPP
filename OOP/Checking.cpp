#include "Checking.h"
#include <iostream>

Checking::~Checking() { std::cout << "~Checking()" << GetName() << std::endl; }

void Checking::Withdraw(float amount)
{
    std::cout << __PRETTY_FUNCTION__ << std::endl;
    if (m_Balance - amount >= 50) {
        Account::Withdraw(amount);
    } else {
        std::cout << "Insufficient balance" << std::endl;
    }
}

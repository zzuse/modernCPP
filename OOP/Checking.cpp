#include "Checking.h"
#include <iostream>

Checking::~Checking() {}

void Checking::Withdraw(float amount)
{
    if (m_Balance - amount >= 50) {
        Account::Withdraw(amount);
    } else {
        std::cout << "Insufficient balance" << std::endl;
    }
}

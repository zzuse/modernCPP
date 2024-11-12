#include "Account.h"
#include <iostream>

int Account::s_ANGenerator = 1000;
Account::Account(const std::string &name, float balance)
    : m_Name(name)
    , m_Balance(balance)
    , m_AccountNumber(s_ANGenerator++)
{
    std::cout << "Account(const std::string, float)" << GetName() << std::endl;
}

Account::~Account() { std::cout << "~Account()" << GetName() << std::endl; }

const std::string &Account::GetName() const { return m_Name; }
float Account::GetBalance() const { return m_Balance; }
int Account::GetAccountNumber() const { return m_AccountNumber; }

void Account::AccumulateInterest() {}
void Account::Withdraw(float amount)
{
    std::cout << __PRETTY_FUNCTION__ << std::endl;
    if (amount > 0 && amount <= m_Balance) {
        m_Balance -= amount;
    } else {
        std::cout << "Insufficient balance" << std::endl;
    }
}
void Account::Deposit(float amount)
{
    std::cout << __PRETTY_FUNCTION__ << std::endl;
    if (amount > 0) {
        m_Balance += amount;
    }
}
float Account::GetInterestRate() const
{
    std::cout << __PRETTY_FUNCTION__ << std::endl;
    return 0.0f;
}
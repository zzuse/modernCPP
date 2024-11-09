#include "Account.h"
#include <iostream>

int Account::s_ANGenerator = 1000;
Account::Account(const std::string &name, float balance)
    : m_Name(name)
    , m_Balance(balance)
    , m_AccountNumber(s_ANGenerator++)
{
}

Account::~Account() {}

const std::string &Account::GetName() const { return m_Name; }
float Account::GetBalance() const { return m_Balance; }
int Account::GetAccountNumber() const { return m_AccountNumber; }

void Account::AccumulateInterest() {}
void Account::Withdraw(float amount)
{
    if (amount > 0 && amount <= m_Balance) {
        m_Balance -= amount;
    } else {
        std::cout << "Insufficient balance" << std::endl;
    }
}
void Account::Deposit(float amount)
{
    if (amount > 0) {
        m_Balance += amount;
    }
}
float Account::GetInterestRate() const { return 0.0f; }
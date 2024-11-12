#pragma once

#include "Account.h"

class Checking : public Account {
    float m_MinimumBalance;

public:
    // Inherit constructor, for pure child class
    using Account::Account;
    Checking(const std::string &name, float balance, float minBalance = 50);
    ~Checking();

    float GetMinimumBalance() const;
    void Withdraw(float amount) override final;
};

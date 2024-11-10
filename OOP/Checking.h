#pragma once

#include "Account.h"

class Checking : public Account {

public:
    // Inherit constructor, for pure child class
    using Account::Account;
    ~Checking();

    void Withdraw(float amount);
};

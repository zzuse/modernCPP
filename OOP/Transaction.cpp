#include "Transaction.h"
#include "Checking.h"
#include <iostream>

void Transaction(Account *pAccount)
{
    std::cout << "Transaction started" << std::endl;
    std::cout << "Balance: initial: " << pAccount->GetBalance() << std::endl;
    pAccount->Deposit(100);
    std::cout << "Balance: After deposit: " << pAccount->GetBalance() << std::endl;
    pAccount->AccumulateInterest();
    std::cout << "Balance: After interest: " << pAccount->GetBalance() << std::endl;
    if (typeid(*pAccount) == typeid(Checking)) {
        std::cout << "typeid static_cast Minimum balance: " << static_cast<Checking *>(pAccount)->GetMinimumBalance()
                  << std::endl;
    }
    Checking *pChecking = dynamic_cast<Checking *>(pAccount);
    if (pChecking != nullptr) {
        std::cout << "dynamic_cast Minimum balance: " << pChecking->GetMinimumBalance() << std::endl;
    }
    pAccount->Withdraw(170);
    std::cout << "Balance: After withdraw: " << pAccount->GetBalance() << std::endl;
    pAccount->GetInterestRate();
    std::cout << "Transaction ended" << std::endl;
}

void Transaction(Account &rAccount)
{
    std::cout << "Transaction started" << std::endl;
    std::cout << "Balance: initial: " << rAccount.GetBalance() << std::endl;
    rAccount.Deposit(100);
    std::cout << "Balance: After deposit: " << rAccount.GetBalance() << std::endl;
    rAccount.AccumulateInterest();
    std::cout << "Balance: After interest: " << rAccount.GetBalance() << std::endl;
    try {
        if (typeid(rAccount) == typeid(Checking)) {
            std::cout << "typeid static_cast Minimum balance: " << static_cast<Checking &>(rAccount).GetMinimumBalance()
                      << std::endl;
        }
        Checking &rChecking = dynamic_cast<Checking &>(rAccount);
        std::cout << "dynamic_cast Minimum balance: " << rChecking.GetMinimumBalance() << std::endl;
    } catch (std::bad_cast &e) {
        std::cout << "cast failed: " << e.what() << std::endl;
        return;
    }
    rAccount.Withdraw(170);
    std::cout << "Balance: After withdraw: " << rAccount.GetBalance() << std::endl;
    rAccount.GetInterestRate();
    std::cout << "Transaction ended" << std::endl;
}

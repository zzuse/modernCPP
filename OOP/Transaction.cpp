#include "Transaction.h"
#include <iostream>

void Transaction(Account* pAccount)
{
    std::cout << "Transaction" << std::endl;
    pAccount->Deposit(100);
    std::cout << "Balance: After deposit: " << pAccount->GetBalance() << std::endl;
    pAccount->AccumulateInterest();
    std::cout << "Balance: After interest: " << pAccount->GetBalance() << std::endl;
    pAccount->Withdraw(170);
    std::cout << "Balance: After withdraw: " << pAccount->GetBalance() << std::endl;
}

#include "Account.h"
#include "Checking.h"
#include "Savings.h"
#include <iostream>
#include <string>
class Animal {
public:
    void Speak() { std::cout << "Animal speaking" << std::endl; }
    void Eat() { std::cout << "Animal eating" << std::endl; }
    void Run() { std::cout << "Animal running" << std::endl; }
};

class Dog : public Animal {
public:
    void Eat() { std::cout << "Dog eating" << std::endl; }
    void Speak() { std::cout << "Dog barking" << std::endl; }
};

int main()
{
    Dog d;
    d.Speak();
    d.Eat();
    d.Run();

    Account acc("John", 1000);
    std::cout << "Initial balance: " << acc.GetBalance() << std::endl;
    acc.Deposit(200);
    std::cout << "Balance after deposit: " << acc.GetBalance() << std::endl;
    acc.Withdraw(380);
    std::cout << "Balance after withdraw: " << acc.GetBalance() << std::endl;

    Savings sav("Jane", 1000, 0.05);
    std::cout << "Initial balance: " << sav.GetBalance() << std::endl;
    sav.Deposit(200);
    std::cout << "Balance after deposit: " << sav.GetBalance() << std::endl;
    sav.AccumulateInterest();
    std::cout << "Balance after interest: " << sav.GetBalance() << std::endl;
    sav.Withdraw(380);
    std::cout << "Balance after withdraw: " << sav.GetBalance() << std::endl;

    Checking ch("Bob", 1000);
    std::cout << "Balance beforewithdraw: " << ch.GetBalance() << std::endl;
    ch.Withdraw(980);
    std::cout << "Balance after withdraw: " << ch.GetBalance() << std::endl;
    return 0;
}

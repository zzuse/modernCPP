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
    return 0;
}

//
// Created by 张臻 on 2024-06-24.
// Revised by 张臻 on 2024-09-10.
//
/* Rule of 5
Destructor
Copy Constructor
Copy assignment operator
Move Constructor
Move assignment operator
*/

#ifndef COPYCTOR_INTEGER_H
#define COPYCTOR_INTEGER_H
#include <iostream>
class Integer {
    int *m_pInt;

public:
    // Default constructor
    Integer();
    // Parameterized constructor
    Integer(int value);
    // Copy constructor
    Integer(const Integer &obj);
    // Move constructor
    Integer(Integer &&obj);

    int GetValue() const;
    void SetValue(int value);

    // Destructor
    ~Integer();

    // Copy assignment
    Integer &operator=(const Integer &obj);
    // Move assignment
    Integer &operator=(Integer &&obj);

    Integer &operator++();
    Integer operator++(int);
    Integer operator+(const Integer &a) const;
    bool operator==(const Integer &a) const;
    void operator()();
    explicit operator int();
};

Integer operator+(int x, const Integer &y);
std::ostream &operator<<(std::ostream &out, const Integer &a);
std::istream &operator>>(std::istream &input, Integer &a);

#endif // COPYCTOR_INTEGER_H

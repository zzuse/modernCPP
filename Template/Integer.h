//
// Created by 张臻 on 2024-06-24.
// Rule of 5
// Destructor
// Copy Constructor
// Copy assignment operator
// Move Constructor
// Move assignment operator
//

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
    // Copy assignment
    Integer &operator=(const Integer &obj);
    // Move assignment
    Integer &operator=(Integer &&obj);
    bool operator==(const Integer &a) const;

    int GetValue() const;
    void SetValue(int value);
    explicit operator int();
    ~Integer();
};

std::ostream &operator<<(std::ostream &out, const Integer &a);

#endif // COPYCTOR_INTEGER_H

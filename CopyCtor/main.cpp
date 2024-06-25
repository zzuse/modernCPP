#include <iostream>
#include "Integer.h"

Integer Add(const Integer &a, const Integer &b) {
    Integer temp;
    temp.SetValue(a.GetValue() + b.GetValue());
    return temp;
}

int main() {
    Integer a(1), b(3);
    a.SetValue(Add(a,b).GetValue());
    std::cout << "Hello, World!" << std::endl;
    return 0;
}

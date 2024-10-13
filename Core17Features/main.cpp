// https://isocpp.org/files/papers/p0636r0.html

#include <iostream>

[[deprecated("Use the new version instead")]] int* CreateIntArray(size_t size) { return new int[size]; }

template <typename T>
[[nodiscard]] T* CreateArray(size_t size)
{
    return new T[size];
}

namespace [[deprecated("DO NOT USE")]] A {
}

class [[deprecated("This class is replaced by NewTest class")]] Test{};

class [[nodiscard]] Number {
};

Number GetNumber(int x) { return Number{}; }

int main()
{
    CreateIntArray(3);
    Test t;
    using namespace A;
    CreateArray<int>(3);
    GetNumber(3);
    return 0;
}
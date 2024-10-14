// https://isocpp.org/files/papers/p0636r0.html

#ifdef __has_include
#if __has_include(<filesystem>)
#include <filesystem>
namespace fs = std::filesystem;
#else
#include <experimental/filesystem>
#endif
#endif

#include <iostream>

[[deprecated("Use the new version instead")]] int* CreateIntArray(size_t size) { return new int[size]; }

template <typename T>
[[nodiscard]] T* CreateArray(size_t size)
{
    return new T[size];
}

namespace [[deprecated("DO NOT USE")]] A {
}

class [[deprecated("This class is replaced by NewTest class")]] Test {};

class [[nodiscard]] Number {};

Number GetNumber(int x) { return Number{}; }

int main()
{
    CreateIntArray(3);
    Test t;
    using namespace A;
    CreateArray<int>(3);
    GetNumber(3);
    fs::path p{"/mnt/data"};
    std::cout << __cpp_inline_variables << std::endl;
    std::cout << __cpp_capture_star_this << std::endl;
    return 0;
}
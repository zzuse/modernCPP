// https://isocpp.org/files/papers/p0636r0.html

#ifdef __has_include
#if __has_include(<filesystem>)
#include <filesystem>
namespace fs = std::filesystem;
#else
#include <experimental/filesystem>
#endif
#endif

#include <fstream>
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

void Write(const std::string& data)
{
    if (std::ofstream out{"./file.txt"}; out && !data.empty()) {
        std::cout << "Writing data into the file\n";
        out << data;
    } else {
        std::cout << "Not data to write\n";
        out << "####";
    }
}

class FileInfo {
public:
    enum Type { Executable, Text };
    Type GetFileType() const { return {}; }
    size_t GetFileSize() const { return 0; }
};

FileInfo GetInfo(const std::string& file) { return {}; }

void Operate(const std::string& file)
{
    switch (auto info = GetInfo(file); info.GetFileType()) {
        case FileInfo::Executable:
            break;
        case FileInfo::Text:
            break;
        default:
            break;
    }
}

namespace A { namespace B { namespace C {

}}}

namespace A::B::C {
    void Foo() {}
}

void Boo() noexcept {}
void Bar() {}

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

    // if (initialization; condition) {}
    if (int* p = (int*)malloc(sizeof(int)); p != nullptr) {
        *p = 50;
        free(p);
    } else {
        // p will be nullptr
    }
    Write("a");
    A::B::C::Foo();
    using A::B::C::Foo;
    using namespace A::B::C;
    Foo();
    void (*func)() noexcept;
    func = Boo;
    // func = Bar; // error for noexcept signature
    func();
    return 0;
}
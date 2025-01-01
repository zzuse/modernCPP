// INSTALL: sudo port install gcc13
// COMPILE: /opt/local/bin/g++-mp-13 -std=c++20 -o main main.cpp
// There are three alternatives for parallel stls execution policy:
// 1. Using poolstl/poolstl.hpp, std::sort(poolstl::par, dataset.begin(), dataset.end());
// 2. Using tbb::parallel_sort(dataset.begin(), dataset.end());
// 3. Using std::sort(std::execution::par, dataset.begin(), dataset.end()); but need gcc-13 support
// Other method not working: tried clang++-mp-17, not support execution policy.
// Another method not working: used tbb as stl backend, compile error, not easy to work with c++20. I don't have
// ablility to dig any more.
//
// #include <poolstl/poolstl.hpp>
// #include "tbb/parallel_sort.h"
// #include "tbb/tbb.h"
#include <algorithm>
#include <any>
#include <chrono>
#include <cstring>
#include <execution>
#include <filesystem>
#include <iostream>
#include <optional>
#include <random>
#include <ratio>
#include <string_view>
#include <variant>
#include <vector>

namespace fs = std::filesystem;

const char* GetErrorString(int errorNo)
{
    switch (errorNo) {
        case 0:
            return "Invalid input";
        case 1:
            return "Connection not established yet";
        default:
            return "";
    }
}

std::optional<const char*> GetErrorStringOp(int errorNo)
{
    switch (errorNo) {
        case 0:
            return "Invalid input";
        case 1:
            return "Connection not established yet";
        default:
            return std::nullopt;
    }
}

class ContactName {
    std::string first;
    std::optional<std::string> middle;
    std::string last;

public:
    ContactName(const std::string& f, const std::optional<std::string>& m, const std::string& l)
        : first{f}
        , middle{m}
        , last{l}
    {
    }

    void Print()
    {
        std::cout << first << " ";
        std::cout << middle.value_or("") << " ";
        std::cout << last << std::endl;
    }
};

struct Number {
    int m_Num{};
    Number(int num)
        : m_Num{num}
    { // on windows __FUNCSIG__
        std::cout << __PRETTY_FUNCTION__ << std::endl;
    }
    Number() { std::cout << __PRETTY_FUNCTION__ << std::endl; }
    ~Number() { std::cout << __PRETTY_FUNCTION__ << std::endl; }
    Number(const Number& other)
    {
        m_Num = other.m_Num;
        std::cout << __PRETTY_FUNCTION__ << std::endl;
    }
    Number(Number&& other) noexcept
    {
        m_Num = other.m_Num;
        other.m_Num = 0;
        std::cout << __PRETTY_FUNCTION__ << std::endl;
    }
    Number& operator=(const Number& other)
    {
        std::cout << __PRETTY_FUNCTION__ << std::endl;
        if (this == &other) return *this;
        m_Num = other.m_Num;
        return *this;
    }
    Number& operator=(Number&& other) noexcept
    {
        std::cout << __PRETTY_FUNCTION__ << std::endl;
        if (this == &other) return *this;
        m_Num = other.m_Num;
        other.m_Num = 0;
        return *this;
    }
    friend std::ostream& operator<<(std::ostream& out, const Number& n) { return out << n.m_Num; }
};

struct Visitor {
    void operator()(const std::string& s) const { std::cout << "string:" << s << std::endl; }
    void operator()(int x) const { std::cout << "int:" << x << std::endl; }
    void operator()(const Number& n) const { std::cout << "Number:" << n << std::endl; }
};

struct Modifier {
    void operator()(std::string& s) const { s += " modified string"; }
    void operator()(int& x) const { x += 1000; }
    void operator()(Number& n) const { n = 999; }
};

void PrettyPrint(std::string_view message, char ch)
{
    for (int x = 0; x < message.length(); ++x) {
        std::cout << ch;
    }
    std::cout << '\n';
    std::cout << message << '\n';
}

enum class Title { MR, MRS, MS };
std::string& CombineName(std::string& name, std::string_view last, Title s)
{
    switch (s) {
        case Title::MR:
            name.insert(0, "Mr.");
            break;
        case Title::MRS:
            name.insert(0, "Mrs.");
            break;
        case Title::MS:
            name.insert(0, "Ms.");
            break;
    }
    return name += last;
}

class Person {
    std::string m_Name;

public:
    Person(std::string name)
        : m_Name{std::move(name)}
    {
    }
    void Print() const { std::cout << m_Name << std::endl; }
};

Person CreatePerson(std::string_view name)
{
    std::string n{name};
    Person p{n};
    return p;
}

void WithClass()
{
    using namespace std::string_literals;
    Person p{CreatePerson("Bruce Wayne")};
    p.Print();
}

void UsingPath()
{
    fs::path selectedPath{R"(/Users/zhangzhen/Documents/Code/Self/modernCPP/STL17Components/build/abc.docx)"};
    std::cout << selectedPath << std::endl;
    std::cout << selectedPath.string() << std::endl;

    if (selectedPath.has_root_name()) {
        std::cout << "root name\t = " << selectedPath.root_name().string() << std::endl;
    }
    if (selectedPath.has_root_path()) {
        std::cout << "root path\t = " << selectedPath.root_path().string() << std::endl;
    }
    if (selectedPath.has_root_directory()) {
        std::cout << "root directory\t = " << selectedPath.root_directory().string() << std::endl;
    }
    if (selectedPath.has_parent_path()) {
        std::cout << "parent path\t = " << selectedPath.parent_path().string() << std::endl;
    }
    if (selectedPath.has_relative_path()) {
        std::cout << "relative path\t = " << selectedPath.relative_path().string() << std::endl;
    }
    if (selectedPath.has_filename()) {
        std::cout << "filename\t = " << selectedPath.filename().string() << std::endl;
    }
    if (selectedPath.has_stem()) {
        std::cout << "stem\t = " << selectedPath.stem().string() << std::endl;
    }
    if (selectedPath.has_extension()) {
        std::cout << "extension\t = " << selectedPath.extension().string() << std::endl;
    }
    selectedPath.remove_filename();
    std::cout << selectedPath << std::endl;
    selectedPath /= "newfile";
    std::cout << selectedPath << std::endl;
}

void TravelsingDirectory(std::string_view file)
{
    fs::path currentPath(file);
    std::vector<fs::directory_entry> dir_entries{};
    fs::directory_iterator begin{currentPath};
    fs::directory_iterator end{};
    while (begin != end) {
        auto de = *begin++;
        std::cout << de.path().filename() << std::endl;
    }
    for (const auto& dir_entry : fs::directory_iterator(currentPath)) {
        std::cout << dir_entry.path() << std::endl;
        dir_entries.push_back(dir_entry);
    }
    std::partition(dir_entries.begin(), dir_entries.end(), [](const fs::directory_entry& de) {
        // return de.is_block_file();
        // return de.is_fifo();
        // return de.is_symlink();
        return de.is_directory();
    });
    for (const auto& dir_entry : dir_entries) {
        switch (const auto& p = dir_entry.path(); fs::status(p).type() /* or: dir_entry.status().type() */) {
            case fs::file_type::directory:
                std::cout << "[DIR]\t" << p.string() << std::endl;
                break;
            case fs::file_type::regular:
                std::cout << '\t' << p.string() << '\t' << dir_entry.file_size() << std::endl;
                break;
            default:
                break;
        }
    }
}

void DirectoryOperations(std::string_view file)
{
    fs::path currentPath{file};
    if (!fs::exists(currentPath)) {
        std::cout << "Path does not exist = >" << currentPath.string() << std::endl;
        return;
    }
    currentPath /= "NewDir";
    if (!fs::create_directory(currentPath)) {
        std::cout << "Could not create a directory\n";
    } else {
        std::cout << "Directory created successfully\n";
    }
    if (!fs::remove(currentPath)) {
        std::cout << "Could not delete the directory\n";
    } else {
        std::cout << "Directory removed successfully\n";
    }
    try {
        std::cout << fs::current_path() << std::endl;
        std::cout << "Changing path\n";
        std::error_code ec{};
        fs::current_path(currentPath, ec);
        if (ec) {
            std::cout << "Error: " << ec.message() << std::endl;
            return;
        }
        std::cout << fs::current_path() << std::endl;
    } catch (std::exception& ex) {
        std::cout << "Exception: " << ex.what() << std::endl;
    }
}

void demo_perms(fs::perms p)
{
    std::cout << ((p & fs::perms::owner_read) != fs::perms::none ? "r" : "-")
              << ((p & fs::perms::owner_write) != fs::perms::none ? "w" : "-")
              << ((p & fs::perms::owner_exec) != fs::perms::none ? "x" : "-")
              << ((p & fs::perms::group_read) != fs::perms::none ? "r" : "-")
              << ((p & fs::perms::group_write) != fs::perms::none ? "w" : "-")
              << ((p & fs::perms::group_exec) != fs::perms::none ? "x" : "-")
              << ((p & fs::perms::others_read) != fs::perms::none ? "r" : "-")
              << ((p & fs::perms::others_write) != fs::perms::none ? "w" : "-")
              << ((p & fs::perms::others_exec) != fs::perms::none ? "x" : "-") << '\n';
}

void Permissions(std::string_view file)
{
    fs::path file_to_modify(file);
    if (!fs::exists(file_to_modify)) {
        std::cout << "Path does not exist = >" << file_to_modify.string() << std::endl;
        return;
    }
    auto perm = fs::status(file_to_modify).permissions();
    demo_perms(perm);
    std::cout << "Changeing permissions\n";
    fs::permissions(file_to_modify, fs::perms::owner_write, fs::perm_options::remove);
    perm = fs::status(file_to_modify).permissions();
    demo_perms(perm);
    std::cout << "Changeing permissions\n";
    fs::permissions(file_to_modify, fs::perms::owner_write, fs::perm_options::add);
    perm = fs::status(file_to_modify).permissions();
    demo_perms(perm);
}

class Timer {
    std::chrono::steady_clock::time_point m_start;

public:
    Timer()
        : m_start{std::chrono::steady_clock::now()}
    {
    }
    void ShowResult(std::string_view message = "")
    {
        auto end = std::chrono::steady_clock::now();
        auto difference = end - m_start;
        std::cout << message << ':' << std::chrono::duration_cast<std::chrono::milliseconds>(difference).count()
                  << '\n';
    }
};

constexpr unsigned VEC_SIZE{10000000};
std::vector<long> CreateVector()
{
    std::vector<long> vec;
    vec.reserve(VEC_SIZE);
    std::default_random_engine engine{std::random_device{}()};
    std::uniform_int_distribution<long> dist(0, VEC_SIZE);
    for (unsigned i = 0; i < VEC_SIZE; ++i) {
        vec.push_back(dist(engine));
    }
    return vec;
}

const size_t testSize = 1000000;
const int iterationCount = 5;

void print_results(const char* const tag, const std::vector<double>& sorted,
                   std::chrono::high_resolution_clock::time_point startTime,
                   std::chrono::high_resolution_clock::time_point endTime)
{
    printf("%s: Lowest: %g Highest: %g Time: %fms\n", tag, sorted.front(), sorted.back(),
           std::chrono::duration_cast<std::chrono::duration<double, std::milli>>(endTime - startTime).count());
}

void one_million_sort()
{
    std::random_device rd;
    printf("Testing with %zu doubles...\n", testSize);
    std::vector<double> doubles(testSize);
    for (auto& d : doubles) {
        d = static_cast<double>(rd());
    }
    for (int i = 0; i < iterationCount; ++i) {
        std::vector<double> sorted(doubles);
        const auto startTime = std::chrono::high_resolution_clock::now();
        sort(sorted.begin(), sorted.end());
        const auto endTime = std::chrono::high_resolution_clock::now();
        print_results("Serial STL", sorted, startTime, endTime);
    }
    for (int i = 0; i < iterationCount; ++i) {
        std::vector<double> sorted(doubles);
        const auto startTime = std::chrono::high_resolution_clock::now();
        std::sort(std::execution::par, sorted.begin(), sorted.end());
        const auto endTime = std::chrono::high_resolution_clock::now();
        print_results("Serial STL", sorted, startTime, endTime);
    }
}

int main()
{
    // std::optional<int> value;
    // std::optional value{5};
    auto value = std::make_optional(5);
    if (value.has_value()) {
        std::cout << value.value() << std::endl;
    }
    if (value) {
        std::cout << *value << std::endl;
    }
    // std::optional reset by assign
    value = 1;
    value.reset();

    auto message = GetErrorString(1);
    if (strlen(message) != 0) {
        std::cout << message << std::endl;
    } else {
        std::cout << "Unknown error number" << std::endl;
    }

    auto errormessage = GetErrorStringOp(0);
    if (errormessage) {
        std::cout << *errormessage << std::endl;
    } else {
        std::cout << "Unknown error number" << std::endl;
    }
    try {
        // std::optional unknow behaviour
        errormessage = GetErrorStringOp(3);
        if (std::nullopt != errormessage) {
            std::cout << *errormessage << std::endl;
        }
        // std::optional safely fail
        errormessage = GetErrorStringOp(4);
        if (std::nullopt != errormessage) {
            std::cout << errormessage.value() << std::endl;
        }
    } catch (std::exception& ex) {
        std::cout << "Exception: " << ex.what() << std::endl;
    }
    // std::optional defalut value
    errormessage = GetErrorStringOp(5);
    std::cout << errormessage.value_or("Default") << std::endl;

    ContactName n1{"Umar", "Majid", "Lone"};
    ContactName n2{"Ayaan", std::nullopt, "Lone"};
    n1.Print();
    n2.Print();

    // std::variant
    try {
        std::variant<std::string, int, Number> v{5};
        std::visit(Modifier{}, v);
        std::visit(Visitor{}, v);
        v = "C++";
        std::visit(Modifier{}, v);
        std::visit(Visitor{}, v);
        v = Number{1};
        std::get<Number>(v) = 100;
        v.emplace<Number>(200);
        std::visit(Modifier{}, v);
        std::visit(Visitor{}, v);
        std::cout << "Access Int Variant" << std::endl;
        v = 5;
        auto val = std::get<int>(v);
        val = std::get<1>(v);
        auto activeIndex = v.index();
        std::cout << activeIndex << std::endl;
        auto p = std::get_if<std::string>(&v);
        if (p == nullptr) {
            std::cout << "Not active" << std::endl;
        } else {
            std::cout << *p << std::endl;
        }

        v = "using lambda";
        auto visitor = [](auto& x) {
            using T = std::decay_t<decltype(x)>;
            if constexpr (std::is_same_v<T, int>) {
                std::cout << "int: " << x << std::endl;
            } else if constexpr (std::is_same_v<T, std::string>) {
                std::cout << "string: " << x << std::endl;
            } else if constexpr (std::is_same_v<T, Number>) {
                std::cout << "Nubmer: " << x << std::endl;
            }
        };
        std::visit(visitor, v);
        v = 555;
        std::visit(visitor, v);
        v = Number(3);
        std::visit(visitor, v);
    } catch (std::exception& ex) {
        std::cout << "Exception: " << ex.what() << std::endl;
    }

    // std::any
    using namespace std::string_literals;
    std::any v1 = 5;
    v1 = "Hello"s;
    try {
        std::cout << std::any_cast<std::string>(v1) << std::endl;
    } catch (std::exception& ex) {
        std::cout << "Exception: " << ex.what() << std::endl;
    }
    v1.reset();
    if (v1.has_value()) {
        if (v1.type() == typeid(int)) {
            std::cout << std::any_cast<int>(v1) << std::endl;
        }
    }
    // std::any number{Number{5}};
    auto number1 = std::make_any<Number>(5);
    number1.reset();
    number1 = 5;
    // reference any_cast
    auto& number2 = std::any_cast<int&>(number1);
    number2 = 100;
    std::cout << std::any_cast<int>(number1) << std::endl;
    // pointer any_cast
    auto pointer = std::any_cast<int>(&number1);
    *pointer = 200;
    std::cout << *std::any_cast<int*>(pointer) << std::endl;

    // string_view
    using namespace std::string_view_literals;
    std::string_view sv1 = "Hello world";
    std::string str1{"Hello"};
    std::string_view sv2 = str1;

    auto sv3 = "Using\0literals"sv;
    std::cout << "Size:" << sv3.length() << std::endl;
    std::cout << sv3 << std::endl;
    std::cout << "data():" << sv3.data() << std::endl;

    std::cout << sv3[0] << std::endl;
    std::cout << sv3.at(0) << std::endl;

    auto f = sv3.find("literals");
    if (f != std::string::npos) {
        std::cout << "Found at index: " << f << std::endl;
    }
    sv3.remove_prefix(3);
    std::cout << sv3 << std::endl;

    sv3.remove_suffix(2);
    std::cout << sv3 << std::endl;

    auto sv4 = sv1.substr(0, 5);
    std::cout << "data() not show substr: " << sv4.data() << std::endl;
    std::cout << sv4 << std::endl;
    PrettyPrint("Hello World", '#');

    std::string name;
    CombineName(name, "Zhang", Title::MR);
    std::cout << name << std::endl;

    WithClass();

    // filesystem
    UsingPath();
    DirectoryOperations(R"(/Users/zhen/Documents/code/modernCPP/STL17Components/build/build)");
    TravelsingDirectory(R"(/Users/zhen/Documents/code/modernCPP/STL17Components/build)");
    Permissions(R"(/Users/zhen/Documents/code/modernCPP/STL17Components/build/CMakeFiles)");

    // Parallel
    auto dataset = CreateVector();
    Timer t1;
    std::sort(dataset.begin(), dataset.end());
    t1.ShowResult("Sequence Sorting time in milliseconds ");
    // parallel stl
    Timer t2;
    // std::sort(poolstl::par, dataset.begin(), dataset.end()); // alternative 1
    // tbb::parallel_sort(dataset.begin(), dataset.end()); // alternative 2
    std::sort(std::execution::par, dataset.begin(), dataset.end());
    t2.ShowResult("Parallel Sorting time in milliseconds ");

    Timer t3;
    std::accumulate(dataset.begin(), dataset.end(), 0);
    t3.ShowResult("sequence Acuumulate time in milliseconds");
    // reduce is parallel stl
    Timer t4;
    std::reduce(dataset.begin(), dataset.end(), 0);
    t4.ShowResult("parallel reduce time in milliseconds");

    one_million_sort();
    return 0;
}
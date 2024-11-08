#include <cassert>
#include <initializer_list>
#include <iostream>
#include <memory>
#include <sstream>
#include <string>

// old style
#define WHITE 0
const int BLACK = 1;
// unscoped enum, Global scope
enum Color : long { RED, BLUE, GREEN };
// scoped enum
enum class TrafficLight : char { RED = 'c', YELLOW, GREEN };
enum class Case { SENSITIVE, INSENSITIVE };

void FillColor(Color color)
{
    if (color == RED)
        std::cout << "Color: "
                  << "RED" << std::endl;
    else if (color == BLUE)
        std::cout << "Color: "
                  << "BLUE" << std::endl;
    else if (color == GREEN)
        std::cout << "Color: "
                  << "GREEN" << std::endl;
}

std::string ToLower(const std::string &str)
{
    std::string returnValue;
    for (const auto &c : str) {
        returnValue += std::tolower(c);
    }
    return returnValue;
}

size_t Find(const std::string &source, const std::string &search_string, Case searchCase, size_t offset = 0);

size_t Find(const std::string &source, const std::string &search_string, Case searchCase = Case::INSENSITIVE,
            size_t offset)
{
    if (searchCase == Case::SENSITIVE) {
        return source.find(search_string, offset);
    } else {
        std::string source_lower = ToLower(source);
        std::string search_string_upper = ToLower(search_string);
        return source_lower.find(search_string_upper, offset);
    }
}

void usingStdString()
{
    std::string s = "Hello ";
    //    std::getline(std::cin, s);
    s.length();
    std::cout << s.length() << std::endl;

    s.insert(6, "World");
    std::cout << s << std::endl;

    // comparison
    if (s == "Hello World") {
        std::cout << s << std::endl;
    }

    // removal
    s.erase(0, 6);
    std::cout << s << std::endl;

    // find
    auto pos = s.find("World");
    if (pos != std::string::npos) {
        std::cout << "Found at position: " << pos << std::endl;
    }

    using namespace std::string_literals;
    auto n2 = "Zen"s;
}

class Distance {
    long double m_Kilometers;

public:
    Distance(long double km)
        : m_Kilometers{km}
    {
    }
    long double GetKm() const { return m_Kilometers; }
    void SetKm(long double val) { m_Kilometers = val; }
};

Distance operator"" _mi(long double val) { return Distance{val * 1.6}; }
Distance operator"" _meters(long double val) { return Distance{val / 1000}; }

class Bag {
    int arr[10];
    int m_Size{};

public:
    Bag(std::initializer_list<int> values)
    {
        assert(values.size() < 10);
        auto it = values.begin();
        while (it != values.end()) {
            Add(*it);
            ++it;
        }
    }
    void Add(int value)
    {
        assert(m_Size < 10);
        arr[m_Size++] = value;
    }
    void Remove() { --m_Size; }
    int operator[](int index) { return arr[index]; }
    int GetSize() const { return m_Size; }
};

void Print(std::initializer_list<int> values)
{
    for (auto x : values) {
        std::cout << x << " ";
    }
}

struct A {
    A() { std::cout << __PRETTY_FUNCTION__ << std::endl; }
    ~A() { std::cout << __PRETTY_FUNCTION__ << std::endl; }
    A(const A &) { std::cout << __PRETTY_FUNCTION__ << std::endl; }
    A(A &&) noexcept { std::cout << __PRETTY_FUNCTION__ << std::endl; }
    A &operator=(const A &other)
    {
        std::cout << __PRETTY_FUNCTION__ << std::endl;
        if (this == &other) {
            return *this;
        }
        return *this;
    }
    A &operator=(A &&other) noexcept
    {
        std::cout << __PRETTY_FUNCTION__ << std::endl;
        if (this == &other) {
            return *this;
        }
        return *this;
    }
};

struct B {
    B() { std::cout << __PRETTY_FUNCTION__ << std::endl; }
    ~B() { std::cout << __PRETTY_FUNCTION__ << std::endl; }
    B(const B &) { std::cout << __PRETTY_FUNCTION__ << std::endl; }
    B(B &&) noexcept { std::cout << __PRETTY_FUNCTION__ << std::endl; }
    B &operator=(const B &other)
    {
        std::cout << __PRETTY_FUNCTION__ << std::endl;
        if (this == &other) {
            return *this;
        }
        return *this;
    }
    B &operator=(B &&other) noexcept
    {
        std::cout << __PRETTY_FUNCTION__ << std::endl;
        if (this == &other) {
            return *this;
        }
        return *this;
    }
    virtual void Foo() { std::cout << __PRETTY_FUNCTION__ << std::endl; }
};

union UDT {
    A a;
    B b;
    std::string s;
    UDT() {}
    ~UDT() {}
};

int main()
{
    Color c = RED;
    FillColor(c);
    FillColor(GREEN);
    FillColor(static_cast<Color>(2));
    int x = static_cast<int>(TrafficLight::RED);
    std::cout << "x: " << x << std::endl;

    usingStdString();

    std::stringstream ss;
    ss << "Sum of " << 5 << " and " << 3 << " is " << 5 + 3 << std::endl;
    std::cout << ss.str();
    // extract numbers from string
    std::string data = "12 89 21";
    ss.str(data);
    int a;
    while (ss >> a) {
        std::cout << a << std::endl;
    }

    std::cout << Find("abcd", "CD", Case::INSENSITIVE) << std::endl;
    // user defined literals
    Distance d1{100.0_mi};
    Distance d2{1000.0_meters};
    std::cout << d1.GetKm() << std::endl;
    std::cout << d2.GetKm() << std::endl;

    // initializer list
    Bag bag{3, 1, 8};
    for (int i = 0; i < bag.GetSize(); ++i) {
        std::cout << bag[i] << " ";
    }
    std::cout << std::endl;
    Print({8, 2, 6, 7});
    std::cout << std::endl;

    // union
    using namespace std::string_literals;
    UDT udt;
    new (&udt.s) std::string("World"s);
    std::cout << udt.s << std::endl;
    new (&udt.a) A();
    udt.a.~A();
    return 0;
}

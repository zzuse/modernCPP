#include "Integer.h"
#include <iostream>
#include <memory>
#include <string>
#include <sstream>

enum class Case{SENSITIVE, INSENSITIVE};

class IntPtr {
    Integer *m_p;

public:
    IntPtr(Integer *p)
        : m_p(p)
    {
    }

    ~IntPtr() { delete m_p; }

    Integer *operator->() { return m_p; }

    Integer &operator*() { return *m_p; }
};
Integer Add(const Integer &a, const Integer &b)
{
    Integer temp;
    temp.SetValue(a.GetValue() + b.GetValue());
    return temp;
}

class Number {
    Integer m_Value{};

public:
    Number() = default;
    Number(int value)
        : m_Value{value}
    {
    }
    Number(const Number &n)
        : m_Value(n.m_Value)
    {
    }
    //    ~Number(){}
    Number(Number &&n)
        : m_Value{std::move(n.m_Value)}
    {
    }
    Number &operator=(Number &&) = default;
    Number &operator=(const Number &) = default;
};

Number CreateNumber(int num)
{
    Number n{num};
    return n;
}

void CreateInteger()
{
    IntPtr p = new Integer;
    p->SetValue(3);
    (*p).SetValue(4);
    std::cout << p->GetValue() << std::endl;
}

class Product {
    Integer m_Id;
public:
    Product(const Integer &id):m_Id{id} {
        std::cout << "Product(const Integer &)" << std::endl;
    }
    ~Product() {
        std::cout << "~";
    }
    const Integer & GetInteger()const {
        return m_Id;
    }
    operator Integer() {
        return m_Id;
    }
};

class Distance {
    long double m_Kilometers;
public:
    Distance(long double km): m_Kilometers{km} {
    }
    long double GetKm() const {
        return m_Kilometers;
    }
    void SetKm(long double val) {
        m_Kilometers = val;
    }
};

Distance operator"" _mi(long double val) {
    return Distance{val * 1.6};
}

void Display(Integer *p) {
    if(!p) {
        return;
    }
    std::cout << p->GetValue() << std::endl;
}

Integer* GetPointer(int value) {
    Integer* p = new Integer{value};
    return p;
}

void Store(std::unique_ptr<Integer>& p ) {
    std::cout << "Storing data into a file:" << p->GetValue() << std::endl;
}

void Operate(int value) {
    std::unique_ptr<Integer> p{GetPointer(value)};
    if (p == nullptr) {
        p.reset(new Integer{value});
    }
    p->SetValue(100);
    Display(p.get());
    p.reset(new Integer{});
    *p = __LINE__;
    Display(p.get());
    Store(p);
    *p = 200;
}

void usingStdString();
size_t Find(const std::string &source,
            const std::string &search_string,
            Case searchCase,
            size_t offset=0);

int main()
{
//    // Example 1
//    Integer a(1), b(3);
//    a.SetValue(Add(a,b).GetValue());
//    // Example 2
//    Number n1{1};
//    auto n2{n1};
//    n2 = n1;
//    std::cout << "Hello, World!" << std::endl;
//
//    auto n3{CreateNumber(3)};
//    n3 = CreateNumber(3);
//
//    // Example 3
//    CreateInteger();
//
//    // Example 4
//    int a = 5, b = 2;
//    float f = static_cast<float>(a) / b;
//    char *p = reinterpret_cast<char *>(&a);
//    const int x = 1;
//    int *q = const_cast<int *>(&x);
//
//    // Example 5
//    // user type to primitive type
//    Integer a1{5};
//    a1 = 7;
//    int x = static_cast<int>(a1);

//    // Example 6
//    Product p{5};
//    Integer id{5};
//    if (id == p.GetInteger()) {
//        std::cout << id.GetValue() << std::endl;
//    }
//    // Example 7
//    Operate(5);
//    // Example 8
//    usingStdString();

//    // Example 9
//    std::stringstream ss;
//    std::string data= "12 89 21";
//    ss.str(data);
//    int a;
//    while(ss >> a) {
//        std::cout << a << std::endl;
//    }
//    std::cout << Find("abcd", "CD",  Case::INSENSITIVE) << std::endl;

//    // Example 10
    Distance dist{32.0_mi};
    std::cout << dist.GetKm() << std::endl;
    return 0;
}

void usingStdString()
{
    std::string s = "Hello ";
//    std::getline(std::cin, s);
    s.length();
    std::cout << s.length() << std::endl;

    s.insert(6, "World");
    std::cout << s << std::endl;

    if (s == "Hello World") {
        std::cout << s << std::endl;
    }

    s.erase(0, 6);
    std::cout << s << std::endl;

    using namespace std::string_literals;
    auto n2 = "Zen"s;
}

std::string ToLower(const std::string& str)
{
    std::string returnValue;
    for (const auto& c : str) {
        returnValue += std::tolower(c);
    }
    return returnValue;
}

size_t Find(
        const std::string &source,
        const std::string &search_string,
        Case searchCase = Case::INSENSITIVE,
        size_t offset)
        {
    if (searchCase == Case::SENSITIVE) {
        return source.find(search_string, offset);
    } else {
        std::string source_lower = ToLower(source);
        std::string search_string_upper = ToLower(search_string);
        return source_lower.find(search_string_upper,offset);
    }
}
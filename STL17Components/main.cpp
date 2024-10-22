#include <iostream>
#include <variant>

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
    // std::optional unknow behaviour
    errormessage = GetErrorStringOp(3);
    std::cout << *errormessage << std::endl;
    // std::optional safely fail
    try {
        errormessage = GetErrorStringOp(4);
        std::cout << errormessage.value() << std::endl;
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
    return 0;
}
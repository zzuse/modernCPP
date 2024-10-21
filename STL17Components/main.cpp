#include <iostream>

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
    return 0;
}
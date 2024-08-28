#include <fstream>
#include <iostream>
#include <string>

void Write()
{
    std::ofstream out("test.txt");
    if (!out) {
        std::cout << "Could not open file for writing" << std::endl;
        return;
    }
    std::string message("C++ was invented by Bjarne");
    for (char ch : message) {
        out.put(ch);
    }
    out.seekp(5);
    out.put('#');
}

void Read()
{
    std::ifstream input("test.txt");
    if (!input) {
        std::cout << "Source file not found" << std::endl;
        return;
    }
    std::cout << "Current position is: " << input.tellg() << std::endl;
    char ch{};
    while (input.get(ch)) {
        std::cout << ch;
    }
}

int main()
{
    Write();
    Read();
    return 0;
}
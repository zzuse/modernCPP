#include <iostream>
#include <memory>
#include <string>

// old style
#define WHITE 0
const int BLACK = 1;
// unscoped enum, Global scope
enum Color : long { RED, BLUE, GREEN };
// scoped enum
enum class TrafficLight : char { RED = 'c', YELLOW, GREEN };

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

int main()
{
    Color c = RED;
    FillColor(c);
    FillColor(GREEN);
    FillColor(static_cast<Color>(2));
    int x = static_cast<int>(TrafficLight::RED);
    std::cout << "x: " << x << std::endl;
    return 0;
}

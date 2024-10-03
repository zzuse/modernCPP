#include <array>
#include <iostream>
#include <vector>

void Array()
{
    std::cout << "Array()" << std::endl;
    std::array<int, 5> arr{3, 1, 8, 5, 9};
    for (int i = 0; i < arr.size(); ++i) {
        arr[i] = i;
    }
    auto itr = arr.begin();
    std::cout << *itr << std::endl;
    for (auto x : arr) {
        std::cout << x << " ";
    }
    std::cout << arr.data() << std::endl;
}

void Vector()
{
    std::cout << "Vector()" << std::endl;
    std::vector<int> coll{1, 2, 3, 4};
    for (int i = 0; i < 5; ++i) {
        coll.push_back(i * 10);
        std::cout << coll.size() << " ";
    }
    std::cout << std::endl;

    coll[0] = 100;
    for (int i = 0; i < coll.size(); ++i) {
        std::cout << coll[i] << " ";
    }
    std::cout << std::endl;

    coll.insert(coll.begin(), 700);
    coll.erase(coll.end() - 5);
    coll.pop_back();
    auto iter = coll.begin();
    while (iter != coll.end()) {
        std::cout << *iter++ << " ";
    }
}

int main()
{
    Array();
    Vector();
    return 0;
}
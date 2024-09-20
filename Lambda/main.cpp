#include <iostream>
#include <string>

using Comparator = bool (*)(int, int);
template <typename T, int size>
void Sort(T (&arr)[size], Comparator comp)
{
    for (int i = 0; i < size - 1; ++i) {
        for (int j = 0; j < size - 1; ++j) {
            if (comp(arr[j], arr[j + 1])) {
                T temp = arr[j];
                arr[j] = arr[j + 1];
                arr[j + 1] = temp;
            }
        }
    }
}

bool Comp(int x, int y) { return x > y; }
bool Comp1(int x, int y) { return x < y; }

int main()
{
    int arr[]{1, 6, 8, 4, 0};
    for (auto x : arr) {
        std::cout << x << " ";
    }
    std::cout << std::endl;
    Sort(arr, Comp1);
    for (auto x : arr) {
        std::cout << x << " ";
    }
    std::cout << std::endl;
    return 0;
}
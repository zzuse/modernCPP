#include <iostream>
#include <string>

template <typename T, int size>
void Sort(T (&arr)[size])
{
    for (int i = 0; i < size - 1; ++i) {
        for (int j = 0; j < size - 1; ++j) {
            if (arr[j] > arr[j + 1]) {
                T temp = arr[j];
                arr[j] = arr[j + 1];
                arr[j + 1] = temp;
            }
        }
    }
}

int main()
{
    int arr[]{1, 6, 8, 4, 0};
    for (auto x : arr) {
        std::cout << x << " ";
    }
    std::cout << std::endl;
    Sort(arr);
    for (auto x : arr) {
        std::cout << x << " ";
    }
    std::cout << std::endl;
    return 0;
}
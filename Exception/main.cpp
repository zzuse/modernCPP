
#include <iostream>

int NewProcessRecords(long long count)
{
    if (count < 10) throw std::out_of_range("Count should be greater than 10");
    int *p = new int[count];
    std::cout << "Success new " << count << std::endl;
    delete[] p;
    return 0;
}

int ProcessRecords(int count)
{
    if (count < 10) throw std::out_of_range("Count should be greater than 10");
    int *pArray = (int *)malloc(count * sizeof(int));
    if (pArray == nullptr) {
        throw std::runtime_error("Failed to allocate memory");
    }
    std::cout << "Success malloc " << count << std::endl;
    for (int i = 0; i < count; ++i) {
        pArray[i] = i;
    }
    free(pArray);
    return 0;
}

int main()
{
    try {
        try {
            ProcessRecords(5);
        } catch (std::out_of_range &ex) {
            std::cout << ex.what() << std::endl;
        }
        try {
            NewProcessRecords(std::numeric_limits<long long>::max());
        } catch (std::bad_alloc &ex) {
            std::cout << ex.what() << std::endl;
        }
        try {
            ProcessRecords(std::numeric_limits<int>::max());
        } catch (std::runtime_error &ex) {
            std::cout << ex.what() << std::endl;
        }
    } catch (std::exception &ex) {
        std::cout << ex.what() << std::endl;
    } catch (...) {
        std::cout << "Exception" << std::endl;
    }
    return 0;
}

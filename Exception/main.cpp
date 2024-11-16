
#include <iostream>
#include <memory>
#include <vector>

class Test {
public:
    Test() { std::cout << "Test()" << std::endl; }
    ~Test() { std::cout << "~Test()" << std::endl; }
};

int NewProcessRecords(long long count)
{
    Test *t = new Test; // leaking memory
    if (count < 10) throw std::out_of_range("Count should be greater than 10");
    int *p = new int[count];
    std::cout << "Success new " << count << std::endl;
    delete[] p;
    delete t;
    return 0;
}

int ContainerProcessRecords(long long count)
{
    std::vector<long long> pArray;
    pArray.reserve(count);
    std::cout << "Success init vector " << count << std::endl;
    for (long long i = 0; i < count; ++i) {
        pArray.push_back(i);
    }
    return 0;
}

int ProcessRecords(int count)
{
    Test t;
    std::unique_ptr<Test> t2(new Test);
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
        try {
            ContainerProcessRecords(std::numeric_limits<long long>::max());
        } catch (std::bad_alloc &ex) {
            std::cout << ex.what() << std::endl;
        }
    } catch (std::exception &ex) {
        std::cout << "What makes you here" << std::endl;
        std::cout << ex.what() << std::endl;
    } catch (...) {
        std::cout << "Exception" << std::endl;
    }
    return 0;
}

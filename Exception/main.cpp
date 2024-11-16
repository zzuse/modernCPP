
#include <iostream>
#include <memory>
#include <random>
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

// Nested Exception
int ContainerProcessRecords(long long count)
{
    std::vector<long long> pArray;
    pArray.reserve(count);
    std::cout << "Success init vector " << count << std::endl;
    for (long long i = 0; i < count; ++i) {
        pArray.push_back(i);
    }
    std::default_random_engine eng;
    std::bernoulli_distribution dist;
    int errors{};
    for (long long i = 0; i < count; ++i) {
        try {
            std::cout << "Processing record # : " << i << " ";
            if (!dist(eng)) {
                ++errors;
                throw std::runtime_error("Could not process the records");
            }
            std::cout << std::endl;
        } catch (std::runtime_error &ex) {
            std::cout << "[ERROR ] " << ex.what() << "]" << std::endl;
            if (errors > 4) {
                // modify exception
                std::runtime_error err("Too many errors. Abandoning operation");
                ex = err;
                throw;
            }
        }
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

class A {
public:
    A() { std::cout << __PRETTY_FUNCTION__ << std::endl; }
    ~A() { std::cout << __PRETTY_FUNCTION__ << std::endl; }
};
class B {
public:
    B() { std::cout << __PRETTY_FUNCTION__ << std::endl; }
    ~B() { std::cout << __PRETTY_FUNCTION__ << std::endl; }
};
class Test2 {
    A *pA{}; // std::unique_ptr<A> pa{}
    B b{};
    int *pInt{};  // std::unique_ptr<int> pInt{}
    char *pStr{}; // std::string
    int *pArr{};  // std::vector<int> pArr{}

public:
    Test2()
    {
        std::cout << __PRETTY_FUNCTION__ << std::endl;
        pA = new A;     // memory leak use smart pointer instead
        pInt = new int; // to fix using smart pointer
        throw std::runtime_error("Failed to initialize");
        pStr = new char[1000]; // to fix using string instead
        pArr = new int[50000]; // to fix using vector instead
    }
    ~Test2()
    {
        std::cout << __PRETTY_FUNCTION__ << std::endl;
        // desctructor is non throwable, avoid it
        // throw std::runtime_error("Failed to destructor");
        delete pA;
        delete pInt;
        delete[] pStr;
        delete[] pArr;
    }
};

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
            ContainerProcessRecords(10);
        } catch (std::bad_alloc &ex) {
            std::cout << ex.what() << std::endl;
        } catch (std::runtime_error &ex) {
            std::cout << ex.what() << std::endl;
        }
    } catch (std::exception &ex) {
        std::cout << "What makes you here " << ex.what() << std::endl;
    } catch (...) {
        std::cout << "Exception" << std::endl;
    }
    try {
        Test2 t;
    } catch (std::runtime_error &ex) {
        std::cout << ex.what() << std::endl;
    }
    return 0;
}

#include "Integer.h"
#include <iostream>
#include <memory>
#include <string>

class Project {
    std::string m_Name;

public:
    void SetName(const std::string &name) { m_Name = name; }
    void ShowProjectDetails() const { std::cout << "Project: " << m_Name << std::endl; }
};

class Employee {
    Project *m_Project{};

public:
    void SetProject(Project *project) { m_Project = project; }
    const Project *GetProject() const { return m_Project; }
};

class Project_share {
    std::string m_Name;

public:
    void SetName(const std::string &name) { m_Name = name; }
    void ShowProjectDetails() const { std::cout << "Project: " << m_Name << std::endl; }
};

class Employee_share {
    std::shared_ptr<Project_share> m_Project{};

public:
    void SetProject(const std::shared_ptr<Project_share> &project) { m_Project = project; }
    const std::shared_ptr<Project_share> &GetProject() const { return m_Project; }
};

void ShowInfo(const Employee *e)
{
    std::cout << "Employee is working in ";
    e->GetProject()->ShowProjectDetails();
}

void Display(Integer *p)
{
    if (!p) {
        return;
    }
    std::cout << p->GetValue() << std::endl;
}

Integer *GetPointer(int value)
{
    Integer *p = new Integer{value};
    return p;
}

void Store(std::unique_ptr<Integer> &p) { std::cout << "Storing data into a file:" << p->GetValue() << std::endl; }

void Operate(int value)
{
    // factory create
    std::unique_ptr<Integer> p{GetPointer(value)};
    // in case it is null
    if (p == nullptr) {
        // delete hold memory, replace with new memory
        p.reset(new Integer{value});
    }
    p->SetValue(100);
    // return underling pointer
    Display(p.get());
    p.reset(new Integer{});
    *p = __LINE__;
    Display(p.get());
    // if you don't want using std::move, you can passing by reference
    Store(p);
    *p = 200;
}

class Printer {
    std::weak_ptr<int> m_pValue{};

public:
    void SetValue(std::shared_ptr<int> p) { m_pValue = p; }
    void Print() const
    {
        if (m_pValue.expired()) {
            std::cout << "No value" << std::endl;
            return;
        }
        auto sp = m_pValue.lock();
        std::cout << "Value: " << *sp << std::endl;
        std::cout << sp.use_count() << std::endl;
    }
};

class Employee_circle;
class Project_circle {
public:
    std::shared_ptr<Employee_circle> m_Employee;
    Project_circle() { std::cout << "Project Constructor" << std::endl; }
    ~Project_circle() { std::cout << "~Project Destructor" << std::endl; }
};
class Employee_circle {
public:
    // to remove circular reference need change to weak_ptr
    // std::shared_ptr<Project_circle> m_Project;
    std::weak_ptr<Project_circle> m_Project;
    Employee_circle() { std::cout << "Employee Constructor" << std::endl; }
    ~Employee_circle() { std::cout << "~Employee Destructor" << std::endl; }
};

struct Free {
    void operator()(int *p)
    {
        std::cout << *p << " ~Freeing object memory" << std::endl;
        free(p);
    }
};

void FreeCallback(int *p)
{
    std::cout << *p << " ~Freeing callback memory" << std::endl;
    free(p);
}

class Point {
    int m_X{};
    int m_Y{};

public:
    Point(int x, int y)
        : m_X{x}
        , m_Y{y}
    {
    }
};

int main()
{

    // std::unique_ptr
    Operate(5);
    // Sharing Pointers
    Project *prj = new Project{};
    prj->SetName("Video Decoder");
    Employee *e1 = new Employee{};
    e1->SetProject(prj);
    Employee *e2 = new Employee{};
    e2->SetProject(prj);
    Employee *e3 = new Employee{};
    e3->SetProject(prj);
    ShowInfo(e1);
    ShowInfo(e2);
    ShowInfo(e3);
    prj->ShowProjectDetails();
    delete prj;
    delete e1;
    delete e2;
    delete e3;
    // std::shared_ptr
    std::shared_ptr<Project_share> prj_share{new Project_share{}};
    prj_share->SetName("Video Decoder");
    std::shared_ptr<Employee_share> e1_share{new Employee_share{}};
    e1_share->SetProject(prj_share);
    std::shared_ptr<Employee_share> e2_share{new Employee_share{}};
    e2_share->SetProject(prj_share);
    std::shared_ptr<Employee_share> e3_share{new Employee_share{}};
    e3_share->SetProject(prj_share);
    e3_share.reset();
    std::cout << "Project shared count: " << prj_share.use_count() << std::endl;
    prj_share->ShowProjectDetails();

    // std::weak_ptr
    int num{200};
    std::shared_ptr<int> sp{new int{num}};
    Printer p;
    p.SetValue(sp);
    p.Print();
    if (*sp > 19) {
        sp.reset(new int{200});
    }
    p.Print();

    // circular reference
    std::shared_ptr<Employee_circle> emp_c{new Employee_circle{}};
    std::shared_ptr<Project_circle> prj_c{new Project_circle{}};
    emp_c->m_Project = prj_c;
    prj_c->m_Employee = emp_c;

    // unique_ptr custom deleter function object
    std::unique_ptr<int, Free> p_mal1{(int *)malloc(sizeof(int)), Free{}};
    *p_mal1 = 100;
    std::cout << *p_mal1 << std::endl;
    // unique_ptr custom deleter function callback
    std::unique_ptr<int, void (*)(int *)> p_mal2{(int *)malloc(sizeof(int)), FreeCallback};
    *p_mal2 = 200;
    std::cout << *p_mal2 << std::endl;

    // shared_ptr custom deleter function object
    std::shared_ptr<int> p_mal3{(int *)malloc(sizeof(int)), Free{}};
    *p_mal3 = 300;
    std::cout << *p_mal3 << std::endl;
    // shared_ptr custom deleter function callback
    std::shared_ptr<int> p_mal4{(int *)malloc(sizeof(int)), FreeCallback};
    *p_mal4 = 400;
    std::cout << *p_mal4 << std::endl;

    // Dynamic arrays
    std::shared_ptr<int[]> p_arr{new int[5]{1, 2, 3, 4, 5}};
    for (size_t i = 0; i < 5; i++) {
        std::cout << p_arr[i] << ",";
    }
    std::cout << std::endl;
    p_arr[0] = 100;
    for (size_t i = 0; i < 5; i++) {
        std::cout << p_arr[i] << ",";
    }
    std::cout << std::endl;
    {
        // make_shared
        std::shared_ptr<int> p_make{new int{100}};
        std::shared_ptr<int> p_make2 = std::make_shared<int>(100);
        std::shared_ptr<int[]> p_make3 = std::make_shared<int[]>(5);
        p_make3[0] = 100;
        std::shared_ptr<Point> p_make4 = std::make_shared<Point>(1, 2);
    }
    {
        // make_unique
        // std::unique_ptr<int> p{new int{5}}; // not recommended
        std::unique_ptr<int> p_unique = std::make_unique<int>(5);
        std::unique_ptr<int[]> p_unique2 = std::make_unique<int[]>(5);
        std::unique_ptr<Point> p_unique3 = std::make_unique<Point>(1, 2);
    }
}

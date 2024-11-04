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
    int num{};
    std::cin >> num;
    std::shared_ptr<int> sp{new int{num}};
    Printer p;
    p.SetValue(sp);
    p.Print();
    if (*sp > 19) {
        sp.reset(new int{200});
    }
    p.Print();
}

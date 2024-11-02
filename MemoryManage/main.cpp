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
}

#include "Integer.h"
#include <array>
#include <deque>
#include <filesystem>
#include <forward_list>
#include <fstream>
#include <iostream>
#include <list>
#include <map>
#include <set>
#include <unordered_map>
#include <unordered_set>
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
    std::cout << std::endl;
}

void Deque()
{
    std::cout << "Deque()" << std::endl;
    std::deque<int> coll{1, 2, 3, 4};
    for (int i = 0; i < 5; ++i) {
        coll.push_back(i * 10);
        std::cout << coll.size() << " ";
    }
    for (int i = 0; i < 5; ++i) {
        coll.push_front(i * 10);
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
    coll.pop_front();
    auto iter = coll.begin();
    while (iter != coll.end()) {
        std::cout << *iter++ << " ";
    }
    std::cout << std::endl;
}

void List()
{
    std::cout << "List()" << std::endl;

    std::list<int> coll;
    for (int i = 0; i < 5; ++i) {
        coll.push_back(i * 10);
    }
    auto itr = coll.begin();
    while (itr != coll.end()) {
        std::cout << *itr++ << " ";
    }
    std::cout << std::endl;

    itr = coll.begin();
    coll.insert(itr, 800);
    coll.erase(itr);
    itr = coll.begin();
    while (itr != coll.end()) {
        std::cout << *itr++ << " ";
    }
    std::cout << std::endl;
}

void ForwardList()
{
    std::cout << "ForwardList()" << std::endl;

    std::forward_list<int> coll;
    for (int i = 0; i < 10; i++) {
        coll.push_front(i);
    }
    for (auto x : coll) {
        std::cout << x << " ";
    }
    std::cout << std::endl;

    coll.insert_after(coll.begin(), 10);
    for (auto x : coll) {
        std::cout << x << " ";
    }
    std::cout << std::endl;
    coll.erase_after(++coll.begin());
    for (auto x : coll) {
        std::cout << x << " ";
    }
    std::cout << std::endl;
}

void Set()
{
    std::cout << "Set()" << std::endl;

    std::multiset<int, std::greater<int>> s{8, 2, 0, 9, 5};
    s.insert(1);
    s.insert(3);
    s.insert(3);
    s.insert(3);

    auto itr = s.begin();
    while (itr != s.end()) {
        std::cout << *itr++ << " ";
    }
    std::cout << std::endl;

    s.erase(0);
    s.erase(s.begin());
    itr = s.find(9);
    if (itr != s.end()) {
        std::cout << "Element found" << std::endl;
    } else {
        std::cout << "Not Found" << std::endl;
    }
    auto found = s.equal_range(3);
    while (found.first != found.second) {
        std::cout << *found.first++ << " ";
    }
    std::cout << std::endl;
}

void Map()
{
    std::cout << "Map()" << std::endl;
    std::map<int, std::string> m{{1, "Superman"}, {2, "Batman"}, {3, "Green Lantern"}};
    m.insert(std::pair<int, std::string>(8, "Aquaman"));
    m.insert(std::make_pair(6, "Wonder Woman"));
    m[0] = "Flash";
    m[0] = "Kid Flash";
    auto itr = m.begin();
    m.erase(0);

    std::cout << m[10] << std::endl;
    for (const auto &x : m) {
        std::cout << x.first << ":" << x.second << std::endl;
    }
    itr = m.find(1);
    if (itr != m.end()) {
        std::cout << "Found: " << itr->second << std::endl;
    } else {
        std::cout << "Not Found" << std::endl;
    }
    std::cout << std::endl;

    std::cout << "MultiMap()" << std::endl;

    std::multimap<int, std::string> m2{{1, "Superman"}, {2, "Batman"}, {3, "Green Lantern"}};
    m2.insert(std::pair<int, std::string>(8, "Aquaman"));
    m2.insert(std::make_pair(6, "Wonder Woman"));
    m2.insert(std::make_pair(6, "PowerGirl"));
    auto itr2 = m2.begin();
    m2.erase(0);
    auto found = m2.equal_range(6);
    while (found.first != found.second) {
        std::cout << found.first->first << ":" << found.first->second << std::endl;
        found.first++;
    }

    std::cout << std::endl;
}

void UnSet()
{
    std::cout << "Unordered_Set()" << std::endl;

    std::unordered_set<std::string> coll;
    coll.insert("Hulk");
    coll.insert("Batman");
    coll.insert("Green Lantern");
    coll.insert("The Flash");
    coll.insert("Wonder Woman");
    coll.insert("Iron man");
    coll.insert("Wolverine");
    coll.insert("Dr. Strange");
    coll.insert("Hawkman");
    for (const auto x : coll) {
        std::cout << "Bucket #:" << coll.bucket(x) << " contains :" << x << std::endl;
    }
    std::cout << "Bucket count:" << coll.bucket_count() << std::endl;
    std::cout << "Number of elements:" << coll.size() << std::endl;
    std::cout << "Load factor:" << coll.load_factor() << std::endl;

    std::cout << std::endl;
    std::cout << "Unordered_MultiSet()" << std::endl;

    std::unordered_multiset<std::string> coll2;
    coll2.insert("Hulk");
    coll2.insert("Batman");
    coll2.insert("Green Lantern");
    coll2.insert("The Flash");
    coll2.insert("Wonder Woman");
    coll2.insert("Iron man");
    coll2.insert("Iron man");
    coll2.insert("Iron man");
    coll2.insert("Wolverine");
    coll2.insert("Dr. Strange");
    coll2.insert("Hawkman");
    for (const auto x : coll2) {
        std::cout << "Bucket #:" << coll2.bucket(x) << " contains :" << x << std::endl;
    }
    std::cout << "Bucket count:" << coll2.bucket_count() << std::endl;
    std::cout << "Number of elements:" << coll2.size() << std::endl;
    std::cout << "Load factor:" << coll2.load_factor() << std::endl;
    std::cout << std::endl;
}

void UnMap()
{
    std::cout << "Unordered_Map()" << std::endl;

    std::unordered_map<std::string, std::string> coll;
    coll["Batman"] = "Bruce Wayne";
    coll["Superman"] = "Clark Kent";
    coll["Hulk"] = "Bruce Banner";

    for (const auto &x : coll) {
        std::cout << "Bucket #" << coll.bucket(x.first) << "->" << x.first << ":" << x.second << std::endl;
    }
    std::cout << std::endl;

    std::cout << "Unordered_MultiMap()" << std::endl;

    std::unordered_multimap<std::string, std::string> coll2;
    coll2.insert(std::make_pair("Batman", "Bruce Wayne"));
    coll2.insert(std::make_pair("Batman", "Bruce Wayne"));
    coll2.insert(std::make_pair("Superman", "Clark Kent"));
    coll2.insert(std::make_pair("Hulk", "Bruce Banner"));

    for (const auto &x : coll2) {
        std::cout << "Bucket #" << coll2.bucket(x.first) << "->" << x.first << ":" << x.second << std::endl;
    }
    std::cout << std::endl;
}

class Employee {
    std::string m_Name;
    int m_Id;
    std::string m_ProgLang;

public:
    Employee(const std::string &n, int id, const std::string &pl)
        : m_Name(n)
        , m_Id(id)
        , m_ProgLang(pl)
    {
    }
    const std::string &GetName() const { return m_Name; }
    int GetId() const { return m_Id; }
    const std::string &GetProgrammingLanguage() const { return m_ProgLang; }
};

struct EmpIds {
    std::vector<int> m_Ids;
    void operator()(const Employee &emp)
    {
        if (emp.GetProgrammingLanguage() == "C++") m_Ids.push_back(emp.GetId());
    }
};

struct EmployeeHash {
    size_t operator()(const Employee &emp) const
    {
        auto s1 = std::hash<std::string>{}(emp.GetName());
        auto s2 = std::hash<int>{}(emp.GetId());
        std::cout << (s1 ^ s2) << std::endl;
        // for combine hash should use more better way in the document in boost
        // https://www.boost.org/doc/libs/1_55_0/doc/html/hash/combine.html
        // https://www.boost.org/doc/libs/1_86_0/libs/container_hash/doc/html/hash.html#ref_hash_combine
        return s1 ^ s2;
    }
};

struct EmpEquality {
    bool operator()(const Employee &e1, const Employee &e2) const
    {
        return e1.GetId() == e2.GetId() && e1.GetName() == e2.GetName();
    }
};

struct EmpCompare {
    bool operator()(const Employee &e1, const Employee &e2) const { return e1.GetId() < e2.GetId(); }
};

void Hashes()
{
    std::cout << "User Defined Hashes()" << std::endl;
    std::hash<std::string> h;
    std::cout << "Hash: " << h("hello") << std::endl;
    std::unordered_set<Employee, EmployeeHash, EmpEquality> coll;
    coll.insert(Employee{"Umar", 123, "C++"});
    coll.insert(Employee{"Bob", 678, "Java"});
    coll.insert(Employee{"Joey", 612, "C++"});

    for (const auto &x : coll) {
        std::cout << coll.bucket(x) << " " << x.GetId() << ":" << x.GetName() << '\n';
    }
    std::cout << std::endl;
}

void UserDefined()
{
    std::cout << "User Defined Vector Sort()" << std::endl;
    std::vector<Employee> v{Employee{"Umar", 123, "C++"}, Employee{"Bob", 678, "Java"}, Employee{"Joey", 612, "C++"}};
    std::sort(v.begin(), v.end(), [](const auto &e1, const auto &e2) { return e1.GetName() < e2.GetName(); });
    for (const auto &e : v) {
        std::cout << "Id:" << e.GetId() << " | Name:" << e.GetName() << " | Language:" << e.GetProgrammingLanguage()
                  << std::endl;
    }
    std::cout << "User Defined Vector Count()" << std::endl;
    int cppCount = std::count_if(v.begin(), v.end(), [](const auto &e) { return e.GetProgrammingLanguage() == "C++"; });
    std::cout << "Cpp Count:" << cppCount << std::endl;
    std::cout << "User Defined Vector Find()" << std::endl;
    auto iter = std::find_if(v.begin(), v.end(), [](const auto &e) { return e.GetProgrammingLanguage() == "Java"; });
    if (iter != v.end()) {
        std::cout << "Found:" << iter->GetName() << "is a Java Programmer" << std::endl;
    }
    std::cout << "User Defined Vector For Each()" << std::endl;
    std::for_each(v.begin(), v.end(), [](const auto &e) { std::cout << e.GetName() << std::endl; });
    auto foundIds = std::for_each(v.begin(), v.end(), EmpIds());
    for (int id : foundIds.m_Ids) {
        std::cout << "Id:" << id << std::endl;
    }
    std::cout << std::endl;

    std::cout << "User Defined Set Sort()" << std::endl;
    std::set<Employee, EmpCompare> s{Employee{"Umar", 123, "C++"}, Employee{"Bob", 678, "Java"},
                                     Employee{"Joey", 612, "C++"}};
    for (const auto &e : s) {
        std::cout << "Id:" << e.GetId() << " | Name:" << e.GetName() << " | Language:" << e.GetProgrammingLanguage()
                  << std::endl;
    }
    std::cout << std::endl;
}

void ContainerImprove()
{
    // construct inside container directly
    std::cout << "Emplace Back is more convenient" << std::endl;
    std::vector<std::pair<int, std::string>> data;
    data.emplace_back(100, "C++");
    std::cout << std::endl;

    // move operation marked noexcept specifier, then vector will automaticly reallocate using move
    std::vector<Integer> vInt;
    vInt.emplace_back(1);
    vInt.emplace_back(2);
    vInt.emplace_back(3);

    // shrink
    for (size_t i = 0; i < 100; ++i) {
        vInt.emplace_back(i);
    }
    std::cout << "Size:" << vInt.size() << '\n';
    std::cout << "Capacity:" << vInt.capacity() << '\n';
    vInt.erase(vInt.begin(), vInt.end() - 10);
    std::cout << "After erase & shrink_to_fit\n";
    vInt.shrink_to_fit();
    std::cout << "Size:" << vInt.size() << '\n';
    std::cout << "Capacity:" << vInt.capacity() << '\n';

    // buffer management for file using vector
    std::ifstream input{"main.cpp"};
    if (!input) {
        std::cout << "Could not open file\n";
        return;
    }
    auto size = std::filesystem::file_size("main.cpp");
    std::vector<char> buffer;
    buffer.resize(size);
    input.read(buffer.data(), size);
    std::cout << buffer.data() << '\n';
}

template <typename Container>
void Print(const Container &cont, const char *msg = "")
{
    std::cout << msg << ":";
    for (auto a : cont) {
        std::cout << a << ' ';
    }
    std::cout << std::endl;
    ;
}

void ContainerImproveII()
{
    // std::erase c++20
    std::vector<int> vvInt{1, 2, 3, 4, 5};
    std::list<int> lstInt{1, 2, 3, 4, 2};
    std::deque<int> deqInt{1, 2, 3, 4, 3};

    std::erase(vvInt, 2);
    std::erase(lstInt, 2);
    std::erase(deqInt, 2);

    Print(vvInt, "vector erase");
    Print(lstInt, "list erase");
    Print(deqInt, "deque erase");

    std::set<int> data{1, 2, 6, 3};
    data.emplace_hint(data.begin(), 0);
    Print(data, "set contains");
    auto found = data.contains(6);
    if (found) std::cout << "Found\n";

    // find and change in set
    std::set<std::string> names{"Omar", "Ayaan", "Raihann"};
    auto it = names.find("Omar");
    auto name = *it;
    name[0] = 'U';
    names.erase(it);
    names.insert(name);
    Print(names, "change set name[0] to U");
    auto node = names.extract(it);
    node.value()[0] = 'E';
    names.insert(std::move(node));
    Print(names, "change set name[0] to E");
}

int main()
{
    Array();
    Vector();
    Deque();
    List();
    ForwardList();
    Set();
    Map();
    UnSet();
    UnMap();
    Hashes();
    UserDefined();
    ContainerImprove();
    ContainerImproveII();
    return 0;
}
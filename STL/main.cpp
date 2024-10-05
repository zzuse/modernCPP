#include <array>
#include <deque>
#include <forward_list>
#include <iostream>
#include <list>
#include <map>
#include <set>
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

int main()
{
    Array();
    Vector();
    Deque();
    List();
    ForwardList();
    Set();
    Map();
    return 0;
}
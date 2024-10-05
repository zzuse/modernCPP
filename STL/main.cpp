#include <array>
#include <deque>
#include <forward_list>
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
    return 0;
}
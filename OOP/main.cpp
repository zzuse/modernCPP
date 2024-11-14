#include "Account.h"
#include "Checking.h"
#include "Savings.h"
#include "Transaction.h"
#include <iostream>
#include <string>
class Animal {
public:
    void Speak() { std::cout << "Animal speaking" << std::endl; }
    void Eat() { std::cout << "Animal eating" << std::endl; }
    void Run() { std::cout << "Animal running" << std::endl; }
};

class Dog : public Animal {
public:
    void Eat() { std::cout << "Dog eating" << std::endl; }
    void Speak() { std::cout << "Dog barking" << std::endl; }
};

class Document {
public:
    virtual void Serialize(float version) = 0;
};

class Text : public Document {
public:
    void Serialize(float version) override final { std::cout << "Serializing Text" << std::endl; }
};

class RichText : public Text {
public:
    // cannot override because final
};

class XML : public Document {
public:
    void Serialize(float version) override { std::cout << "Serializing XML" << std::endl; }
};

void Write(Document *p) { p->Serialize(1.0f); }

class Stream {
    std::string m_FileName;

public:
    Stream(const std::string &filename)
        : m_FileName(filename)
    {
        std::cout << "Stream(const std::string&)" << std::endl;
    }
    const std::string &GetFileName() const { return m_FileName; }
    ~Stream() { std::cout << "~Stream()" << std::endl; }
};

class OutputStream : virtual public Stream {
    std::ostream &out;

public:
    OutputStream(std::ostream &o, const std::string &filename)
        : out(o)
        , Stream(filename)
    {
        std::cout << "OutputStream" << std::endl;
    }
    std::ostream &operator<<(const std::string &data)
    {
        out << data;
        return out;
    }
    ~OutputStream() { std::cout << "~OutputStream" << std::endl; }
};

class InputStream : virtual public Stream {
    std::istream &in;

public:
    InputStream(std::istream &o, const std::string &filename)
        : in(o)
        , Stream(filename)
    {
        std::cout << "InputStream" << std::endl;
    }
    std::istream &operator>>(std::string &data)
    {
        in >> data;
        return in;
    }
    ~InputStream() { std::cout << "~InputStream" << std::endl; }
};

class IOStream : public OutputStream, public InputStream {
public:
    IOStream(const std::string &filename)
        : OutputStream(std::cout, filename)
        , InputStream(std::cin, filename)
        , Stream(filename)
    {
        std::cout << "IOStream" << std::endl;
    }
    ~IOStream() { std::cout << "~IOStream" << std::endl; }
};

int main()
{
    Dog d;
    d.Speak();
    d.Eat();
    d.Run();

    Account acc("John", 1000);
    std::cout << "Initial balance: " << acc.GetBalance() << std::endl;
    acc.Deposit(200);
    std::cout << "Balance after deposit: " << acc.GetBalance() << std::endl;
    acc.Withdraw(380);
    std::cout << "Balance after withdraw: " << acc.GetBalance() << std::endl;

    Savings sav("Jane", 1000, 0.05);
    std::cout << "Initial balance: " << sav.GetBalance() << std::endl;
    sav.Deposit(200);
    std::cout << "Balance after deposit: " << sav.GetBalance() << std::endl;
    sav.AccumulateInterest();
    std::cout << "Balance after interest: " << sav.GetBalance() << std::endl;
    sav.Withdraw(380);
    std::cout << "Balance after withdraw: " << sav.GetBalance() << std::endl;

    Checking ch("Bob", 1000);
    std::cout << "Minimum balance: " << ch.GetMinimumBalance() << std::endl;
    std::cout << "Balance beforewithdraw: " << ch.GetBalance() << std::endl;
    ch.Withdraw(980);
    std::cout << "Balance after withdraw: " << ch.GetBalance() << std::endl;

    Transaction(&ch);
    Transaction(&sav);

    // virtual destructor
    Account *pAcc = new Savings("Alice", 2000, 0.05);
    delete pAcc;

    Checking ch2("Bob2", 1000, 50);
    // upcast
    Account *pAcc2 = &ch2;
    // downcast
    Checking *pCh = static_cast<Checking *>(pAcc2);
    std::cout << "Minimum balance: " << pCh->GetMinimumBalance() << std::endl;

    // typeid or dynamic_cast
    std::cout << "Type of pAcc2: " << typeid(pAcc2).name() << std::endl;
    std::cout << "Type of pCh: " << typeid(pCh).name() << std::endl;
    Transaction(pCh);
    Savings sav2("Alice", 2000, 0.05);
    pAcc2 = &sav2;
    const std::type_info &ti = typeid(pAcc2);
    std::cout << "Type of pAcc2: " << ti.name() << std::endl;
    const std::type_info &ti2 = typeid(*pAcc2);
    Transaction(pAcc2);
    if (ti2 == typeid(Savings)) {
        std::cout << "Type of *pAcc2: " << ti2.name() << std::endl;
        std::cout << "pAcc2 points to saving objects" << std::endl;
    } else {
        std::cout << "pAcc2 does not points to saving objects" << std::endl;
    }
    Transaction(ch2);
    Transaction(sav2);

    XML xml;
    Write(&xml);

    // multiple inheritance, make sure only one base instance init
    IOStream stream("doc.txt");
    std::cout << stream.GetFileName() << std::endl;
    return 0;
}

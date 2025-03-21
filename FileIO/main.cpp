#include <filesystem>
#include <fstream>
#include <iostream>
#include <string>

// https://en.cppreference.com/w/cpp/header/filesystem
namespace fs = std::filesystem;

struct Record {
    int id;
    char name[10];
};

void WriteRecord(Record *p)
{
    std::ofstream binstream{"records", std::ios::binary | std::ios::out};
    binstream.write((const char *)p, sizeof(Record));
}

Record GetRecord()
{
    std::ifstream input{"records", std::ios::binary | std::ios::in};
    Record r;
    input.read((char *)&r, sizeof(Record));
    return r;
}

void Write()
{
    std::ofstream out("test.txt");
    if (!out) {
        std::cout << "Could not open file for writing" << std::endl;
        return;
    }
    std::string message("C++ was invented by Bjarne");
    for (char ch : message) {
        out.put(ch);
    }
    out.seekp(5);
    out.put('#');
}

void Read()
{
    std::ifstream input("test.txt");
    if (!input) {
        std::cout << "Source file not found" << std::endl;
        return;
    }
    std::cout << "Current position is: " << input.tellg() << std::endl;
    char ch{};
    while (input.get(ch)) {
        std::cout << ch;
    }
}

void usingFstream()
{
    std::fstream stream("file.txt");
    if (!stream) {
        std::cout << "\nFile not exist, creating... " << std::endl;
        std::ofstream out{"file.txt"};
        out.close();
        stream.open("file.txt");
    }
    stream << "Hello World" << std::endl;

    std::string line;
    stream.seekg(0);
    std::getline(stream, line);
    std::cout << std::endl << line << std::endl;
}

int main()
{
    // Example 1, text file IO and seek
    Write();
    Read();
    usingFstream();

    // Example 2, binary file IO and structure RW
    Record r;
    r.id = 1024;
    strncpy(r.name, "Zhen", 10);
    WriteRecord(&r);

    Record r2 = GetRecord();
    std::cout << r2.id << ": " << r2.name << std::endl;

    // Example 3 Text File Copy Utility
    fs::path source(fs::current_path());
    source /= "../main.cpp";

    fs::path dest(fs::current_path());
    dest /= "Copy.cpp";

    std::ifstream input{source};
    if (!input) {
        std::cout << "Source file not found " << std::endl;
        return -1;
    }
    std::ofstream output{dest};
    std::string line;
    while (!std::getline(input, line).eof()) {
        output << line << std::endl;
    }
    input.close();
    output.close();

    // Exmpale 4 Binary File Copy Utility
    fs::path src_binary(fs::current_path());
    src_binary /= "records";

    fs::path dest_binary(fs::current_path());
    dest_binary /= "Copy.bin";

    std::ifstream input_b{src_binary, std::ios::binary | std::ios::in};
    if (!input_b) {
        std::cout << "Source file not found " << std::endl;
        return -1;
    }
    std::ofstream output_b{dest_binary, std::ios::binary | std::ios::out};

    auto fileSize = file_size(src_binary);
    const unsigned int BUFFER_SIZE = 512;
    char buffer[BUFFER_SIZE]{};
    if (!input_b.read(buffer, fileSize)) {
        throw std::runtime_error("Error occurred during read operation");
    }
    if (!output_b.write(buffer, fileSize)) {
        throw std::runtime_error("Error occurred during write operation");
    }

    // Example 5
    fs::current_path(fs::temp_directory_path());
    if (!fs::create_directories("sandbox/a/b")) {
        std::cout << "Could not create sub directory in: " << fs::current_path() << std::endl;
        if (!fs::remove("sandbox/a/b")) {
            std::cout << "Could not delete the directory\n";
        } else {
            std::cout << "Directory removed successfully\n";
        }
    }
    fs::create_directories("sandbox/a/b");
    std::cout << "Directoddry created successfully\n";
    std::ofstream("sandbox/file1.txt");
    fs::remove("sandbox/syma");
    fs::create_symlink("a", "sandbox/syma");
    for (const fs::directory_entry &dir_entry : fs::recursive_directory_iterator("sandbox")) {
        std::cout << dir_entry.path().string() << '\n';
    }
    return 0;
}
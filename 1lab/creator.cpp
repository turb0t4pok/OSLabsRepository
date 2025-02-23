#include <iostream>
#include <fstream>
#include <string>

struct employee {
    int num;
    char name[10];
    double hours;
};

int main(int argc, char* argv[]) {
    bool silentMode = false;

    if (argc > 1 && std::string(argv[argc - 1]) == "--silent") {
        silentMode = true;
        argc--; 
    }

    if (argc != 3) {
        if (!silentMode) std::cerr << "Usage: Creator <filename> <number of records> [--silent]" << std::endl;
        return 1;
    }

    const char* filename = argv[1];
    int recordCount = std::stoi(argv[2]);

    std::ofstream outFile(filename, std::ios::binary);
    if (!outFile) {
        if (!silentMode) std::cerr << "Error opening file for writing!" << std::endl;
        return 1;
    }

    for (int i = 0; i < recordCount; ++i) {
        employee emp;

        if (!silentMode) std::cout << "Enter employee number: ";
        std::cin >> emp.num;

        if (!silentMode) std::cout << "Enter employee name (up to 9 characters): ";
        std::cin >> emp.name;

        if (!silentMode) std::cout << "Enter hours worked: ";
        std::cin >> emp.hours;

        outFile.write(reinterpret_cast<char*>(&emp), sizeof(emp));
    }

    outFile.close();
    if (!silentMode) std::cout << "File created successfully." << std::endl;
    return 0;
}

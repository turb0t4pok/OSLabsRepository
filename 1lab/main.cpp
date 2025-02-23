#include <iostream>
#include <fstream>
#include <string>
#include <windows.h>

void printBinaryFile(const char* filename) {
	std::ifstream inFile(filename, std::ios::binary); 
	if (!inFile) {
		std::cerr << "Error opening binary file!" << std::endl;
		return;
	}

	struct employee {
		int num;
		char name[10];
		double hours;
	};

	employee emp;
	while (inFile.read(reinterpret_cast<char*>(&emp), sizeof(emp))) {
		std::cout << "Employee Number: " << emp.num << ", Name: " << emp.name
			<< ", Hours Worked: " << emp.hours << std::endl;
	}

	inFile.close();
}


void printReportFile(const char* filename) {
	std::ifstream inFile(filename);
	if (!inFile) {
		std::cerr << "Error opening report file!" << std::endl;
		return;
	}

	std::string line;
	while (std::getline(inFile, line)) {
		std::cout << line << std::endl;
	}

	inFile.close();
}

int main() {
    std::string binaryFile, reportFile;
    int recordCount;
    double hourlyRate;

    std::cout << "Enter the name of the binary file: ";
    std::cin >> binaryFile;
    binaryFile += ".bin";

    std::cout << "Enter the number of records: ";
    std::cin >> recordCount;

    STARTUPINFOA si = { sizeof(STARTUPINFOA) };
    PROCESS_INFORMATION pi;
    std::string creatorCmd = "Creator.exe " + binaryFile + " " + std::to_string(recordCount);
    if (!CreateProcessA(NULL, const_cast<LPSTR>(creatorCmd.data()), NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi)) {
        std::cerr << "Failed to launch Creator process!" << std::endl;
        return 1;
    }

    WaitForSingleObject(pi.hProcess, INFINITE);
    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);

    std::cout << "Contents of the binary file:" << std::endl;
    printBinaryFile(binaryFile.c_str());

    std::cout << "Enter the name of the report file: ";
    std::cin >> reportFile;
    reportFile += ".txt";

    std::cout << "Enter the hourly rate: ";
    std::cin >> hourlyRate;

    std::string reporterCmd = "Reporter.exe " + binaryFile + " " + reportFile + " " + std::to_string(hourlyRate);

    if (!CreateProcessA(NULL, const_cast<LPSTR>(reporterCmd.data()), NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi)) {
        std::cerr << "Failed to launch Reporter process!" << std::endl;
        return 1;
    }

    WaitForSingleObject(pi.hProcess, INFINITE);
    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);

    std::cout << "Report contents:" << std::endl;
    printReportFile(reportFile.c_str());

    return 0;
}

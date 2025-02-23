#include <gtest/gtest.h>
#include <fstream>
#include <filesystem>

namespace fs = std::filesystem;

TEST(ReporterTest, CalculatesCorrectSalary) {
    std::string binFile = "test_reporter.bin";
    std::string reportFile = "test_reporter.txt";
    std::string creatorPath = (fs::current_path() / "Creator.exe").string();
    std::string reporterPath = (fs::current_path() / "Reporter.exe").string();
    double payRate = 15.0;

    if (fs::exists(binFile)) fs::remove(binFile);
    if (fs::exists(reportFile)) fs::remove(reportFile);

    std::ofstream inputFile("input.txt");
    inputFile << "1\nJohn\n40.5\n";
    inputFile << "2\nBill\n35\n";
    inputFile.close();

    std::string creatorCmd = creatorPath + " " + binFile + " 2 --silent < input.txt";
    ASSERT_EQ(system(creatorCmd.c_str()), 0) << "Couldn't run Creator.exe!";

    ASSERT_TRUE(fs::exists(binFile)) << "File " << binFile << " was not created.";

    std::string reporterCmd = reporterPath + " " + binFile + " " + reportFile + " " + std::to_string(payRate);
    ASSERT_EQ(system(reporterCmd.c_str()), 0) << "Couldn't run Reporter.exe!";

    ASSERT_TRUE(fs::exists(reportFile)) << "File " << reportFile << " was not created.";

    std::ifstream report(reportFile);
    ASSERT_TRUE(report.is_open()) << "Couldn't open " << reportFile;

    std::string line;
    std::getline(report, line);
    std::getline(report, line);

    ASSERT_FALSE(line.empty()) << "Report file is empty.";
}

#include <gtest/gtest.h>
#include <fstream>
#include <filesystem>
#include <cstdlib>

namespace fs = std::filesystem;

TEST(MainTest, RunsCreatorAndReporter) {
    std::string binFile = "test_main.dat";
    std::string reportFile = "test_main_report.txt";
    double payRate = 20.0;

    std::string creatorPath = (fs::current_path() / "Creator.exe").string();
    std::string reporterPath = (fs::current_path() / "Reporter.exe").string();

    if (fs::exists(binFile)) fs::remove(binFile);
    if (fs::exists(reportFile)) fs::remove(reportFile);

    std::ofstream inputFile("input.txt");
    inputFile << "1\nJohn\n40.5\n";
    inputFile << "2\nBill\n35\n";
    inputFile.close();

    std::string creatorCmd = creatorPath + " " + binFile + " 2 --silent < input.txt";
    ASSERT_EQ(system(creatorCmd.c_str()), 0) << "Creator.exe could not be run.";

    ASSERT_TRUE(fs::exists(binFile)) << "File " << binFile << " was not created.";

    std::string reporterCmd = reporterPath + " " + binFile + " " + reportFile + " " + std::to_string(payRate);
    ASSERT_EQ(system(reporterCmd.c_str()), 0) << "Reporter.exe could not be run.";

    ASSERT_TRUE(fs::exists(reportFile)) << "File " << reportFile << " was not created.";
}

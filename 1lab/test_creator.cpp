#include <gtest/gtest.h>
#include <fstream>
#include <filesystem>
#include <iostream>

namespace fs = std::filesystem;

TEST(CreatorTest, CreatesBinaryFile) {
    std::string binFile = "test_creator.bin";
    std::string creatorPath = (fs::current_path() / "Creator.exe").string();

    if (fs::exists(binFile)) fs::remove(binFile);

    std::ofstream inputFile("input.txt");
    inputFile << "1\nJohn\n40.5\n";
    inputFile << "2\nBill\n35\n";
    inputFile.close();

    std::string creatorCmd = creatorPath + " " + binFile + " 2 --silent < input.txt";
    std::cout << "Executing command: " << creatorCmd << std::endl;

    ASSERT_EQ(system(creatorCmd.c_str()), 0) << "Couldn't run Creator.exe";

    ASSERT_TRUE(fs::exists(binFile)) << "File " << binFile << " was not created.";

    std::ifstream inFile(binFile, std::ios::binary);
    ASSERT_TRUE(inFile.is_open()) << "Couldn't open " << binFile;
}
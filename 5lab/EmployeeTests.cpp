#include <gtest/gtest.h>
#include "EmployeeUtils.h"
#include <fstream>
#include <thread>

// Простая фикстура
class EmployeeTest : public ::testing::Test {
protected:
    std::vector<employee> testEmployees;
    std::string testFileName = "test_employees.bin";

    void SetUp() override {
        testEmployees = {
            {1, "Alice", 40.0},
            {2, "Bob", 35.5},
            {3, "Charlie", 20.0}
        };
        writeToFile(testFileName, testEmployees);
    }

    void TearDown() override {
        std::remove(testFileName.c_str());
    }
};

TEST_F(EmployeeTest, WriteAndReadFile) {
    std::vector<employee> readEmployees;
    readFromFile(testFileName, readEmployees);

    ASSERT_EQ(readEmployees.size(), testEmployees.size());
    for (size_t i = 0; i < testEmployees.size(); ++i) {
        EXPECT_EQ(readEmployees[i].num, testEmployees[i].num);
        EXPECT_STREQ(readEmployees[i].name, testEmployees[i].name);
        EXPECT_DOUBLE_EQ(readEmployees[i].hours, testEmployees[i].hours);
    }
}

TEST_F(EmployeeTest, LockUnlockRead) {
    int id = 1;
    recordLocks[id]; // init
    std::atomic<bool> readDone = false;

    std::thread reader([&]() {
        lockForRead(id);
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        readDone = true;
        unlockForRead(id);
        });

    reader.join();
    ASSERT_TRUE(readDone);
}

TEST_F(EmployeeTest, LockUnlockWriteBlocksRead) {
    int id = 2;
    recordLocks[id];

    std::atomic<bool> writeStarted = false;
    std::atomic<bool> readFinished = false;

    std::thread writer([&]() {
        lockForWrite(id);
        writeStarted = true;
        std::this_thread::sleep_for(std::chrono::milliseconds(200));
        unlockForWrite(id);
        });

    std::thread reader([&]() {
        while (!writeStarted); // wait until write starts
        lockForRead(id);
        readFinished = true;
        unlockForRead(id);
        });

    writer.join();
    reader.join();

    ASSERT_TRUE(readFinished);
}

TEST_F(EmployeeTest, LockUnlockWriteBlocksOtherWrites) {
    int id = 3;
    recordLocks[id];
    std::atomic<bool> writer1Done = false;
    std::atomic<bool> writer2Started = false;

    std::thread writer1([&]() {
        lockForWrite(id);
        std::this_thread::sleep_for(std::chrono::milliseconds(200));
        writer1Done = true;
        unlockForWrite(id);
        });

    std::thread writer2([&]() {
        while (!writer1Done);
        lockForWrite(id); // Should block until writer1 is done
        writer2Started = true;
        unlockForWrite(id);
        });

    writer1.join();
    writer2.join();

    ASSERT_TRUE(writer2Started);
}

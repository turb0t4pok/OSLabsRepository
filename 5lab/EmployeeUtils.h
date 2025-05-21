#pragma once
#include <vector>
#include <string>
#include <fstream>
#include <map>
#include <mutex>
#include <condition_variable>

struct employee {
    int num;
    char name[10];
    double hours;
};

struct RecordLock {
    std::mutex mtx;
    std::condition_variable cv;
    int readers = 0;
    bool writer = false;
};

extern std::map<int, RecordLock> recordLocks;

void writeToFile(const std::string& filename, const std::vector<employee>& emps);
void readFromFile(const std::string& filename, std::vector<employee>& emps);

void lockForRead(int id);
void unlockForRead(int id);
void lockForWrite(int id);
void unlockForWrite(int id);

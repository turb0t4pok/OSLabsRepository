#include "EmployeeUtils.h"

std::map<int, RecordLock> recordLocks;

void writeToFile(const std::string& filename, const std::vector<employee>& emps) {
    std::ofstream file(filename, std::ios::binary);
    file.write(reinterpret_cast<const char*>(emps.data()), emps.size() * sizeof(employee));
}

void readFromFile(const std::string& filename, std::vector<employee>& emps) {
    std::ifstream file(filename, std::ios::binary | std::ios::ate);
    std::streamsize size = file.tellg();
    file.seekg(0, std::ios::beg);

    size_t count = size / sizeof(employee);
    emps.resize(count);
    file.read(reinterpret_cast<char*>(emps.data()), size);
}

void lockForRead(int id) {
    auto& lock = recordLocks[id];
    std::unique_lock<std::mutex> lk(lock.mtx);
    lock.cv.wait(lk, [&] { return !lock.writer; });
    lock.readers++;
}

void unlockForRead(int id) {
    auto& lock = recordLocks[id];
    std::lock_guard<std::mutex> lk(lock.mtx);
    if (--lock.readers == 0) {
        lock.cv.notify_one();
    }
}

void lockForWrite(int id) {
    auto& lock = recordLocks[id];
    std::unique_lock<std::mutex> lk(lock.mtx);
    lock.cv.wait(lk, [&] { return lock.readers == 0 && !lock.writer; });
    lock.writer = true;
}

void unlockForWrite(int id) {
    auto& lock = recordLocks[id];
    std::lock_guard<std::mutex> lk(lock.mtx);
    lock.writer = false;
    lock.cv.notify_all();
}

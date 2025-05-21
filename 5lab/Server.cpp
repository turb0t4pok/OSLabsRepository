#include <windows.h>
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <thread>
#include <mutex>
#include <map>
#include <atomic>
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

std::string fileName;
std::vector<employee> employees;
std::mutex fileMutex;
std::map<int, RecordLock> recordLocks;
HANDLE hPipe = INVALID_HANDLE_VALUE;
std::atomic<bool> serverRunning{ true };
std::atomic<int> activeClients{ 0 };

void printEmployees() {
    std::lock_guard<std::mutex> lock(fileMutex);
    std::cout << "\nCurrent file content:\nID\tName\tHours\n";
    for (const auto& emp : employees) {
        std::cout << emp.num << "\t" << emp.name << "\t" << emp.hours << "\n";
    }
    std::cout << std::endl;
}

void initializeFile() {
    std::cout << "Enter file name: ";
    std::cin >> fileName;

    int count;
    std::cout << "Enter number of employees: ";
    std::cin >> count;

    employees.resize(count);
    for (int i = 0; i < count; ++i) {
        std::cout << "Employee " << i + 1 << ":\nID: ";
        std::cin >> employees[i].num;
        std::cout << "Name: ";
        std::cin >> employees[i].name;
        std::cout << "Hours: ";
        std::cin >> employees[i].hours;

        // Инициализируем блокировки для каждого сотрудника
        recordLocks[employees[i].num]; // Создаем запись в map
    }

    std::ofstream file(fileName, std::ios::binary);
    file.write(reinterpret_cast<const char*>(employees.data()), employees.size() * sizeof(employee));
    file.close();

    printEmployees();
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

void handleClient(HANDLE pipe) {
    ++activeClients;
    char op;
    DWORD bytesRead;

    while (serverRunning && ReadFile(pipe, &op, sizeof(op), &bytesRead, NULL)) {
        if (op == 'R' || op == 'W') {
            int id;
            ReadFile(pipe, &id, sizeof(id), &bytesRead, NULL);

            // Проверка существования записи
            bool exists = false;
            {
                std::lock_guard<std::mutex> fileLock(fileMutex);
                for (const auto& emp : employees) {
                    if (emp.num == id) {
                        exists = true;
                        break;
                    }
                }
            }

            if (!exists) {
                employee empty{ 0 };
                WriteFile(pipe, &empty, sizeof(empty), &bytesRead, NULL);
                continue;
            }

            employee emp{ 0 };

            if (op == 'R') {
                // Блокировка для чтения (блокирует запись, но разрешает другие чтения)
                lockForRead(id);

                // Чтение данных
                {
                    std::lock_guard<std::mutex> fileLock(fileMutex);
                    for (const auto& e : employees) {
                        if (e.num == id) {
                            emp = e;
                            break;
                        }
                    }
                }

                WriteFile(pipe, &emp, sizeof(emp), &bytesRead, NULL);

                // Ждем команду на завершение доступа
                char finish;
                ReadFile(pipe, &finish, sizeof(finish), &bytesRead, NULL);

                unlockForRead(id);
            }
            else if (op == 'W') {
                // Блокировка для записи
                lockForWrite(id);

                // 1. Находим запись в памяти
                employee emp{ 0 };
                bool found = false;
                {
                    std::lock_guard<std::mutex> fileLock(fileMutex);
                    for (const auto& e : employees) {
                        if (e.num == id) {
                            emp = e;
                            found = true;
                            break;
                        }
                    }
                }

                // 2. Отправляем текущие данные клиенту
                WriteFile(pipe, &emp, sizeof(emp), &bytesRead, NULL);

                // 3. Получаем измененные данные
                employee modifiedEmp;
                ReadFile(pipe, &modifiedEmp, sizeof(modifiedEmp), &bytesRead, NULL);

                // 4. Обновляем данные
                {
                    std::lock_guard<std::mutex> fileLock(fileMutex);
                    for (auto& e : employees) {
                        if (e.num == id) {
                            e = modifiedEmp;
                            break;
                        }
                    }

                    // 5. Сохраняем в файл
                    std::ofstream outFile(fileName, std::ios::binary);
                    outFile.write(reinterpret_cast<const char*>(employees.data()),
                        employees.size() * sizeof(employee));
                }

                // 6. Отправляем подтверждение
                const char* response = "OK";
                WriteFile(pipe, response, strlen(response) + 1, NULL, NULL);

                // 7. Ждем команду о завершении доступа
                char finish;
                ReadFile(pipe, &finish, sizeof(finish), &bytesRead, NULL);

                // 8. Снимаем блокировку
                unlockForWrite(id);
            }
        }
    }

    --activeClients;
    DisconnectNamedPipe(pipe);
    CloseHandle(pipe);
}

void runServer() {
    while (serverRunning) {
        hPipe = CreateNamedPipeA(
            "\\\\.\\pipe\\EmployeePipe",
            PIPE_ACCESS_DUPLEX,
            PIPE_TYPE_MESSAGE | PIPE_READMODE_MESSAGE | PIPE_WAIT,
            PIPE_UNLIMITED_INSTANCES,
            1024,
            1024,
            0,
            NULL);

        if (hPipe == INVALID_HANDLE_VALUE) {
            if (serverRunning) std::cerr << "Pipe creation failed: " << GetLastError() << "\n";
            break;
        }

        if (ConnectNamedPipe(hPipe, NULL) || GetLastError() == ERROR_PIPE_CONNECTED) {
            std::thread(handleClient, hPipe).detach();
        }
        else {
            CloseHandle(hPipe);
        }
    }
}

void startClients(int count) {
    for (int i = 0; i < count; ++i) {
        STARTUPINFOA si = { sizeof(si) };
        PROCESS_INFORMATION pi;

        std::string cmd = "cmd.exe /c start \"Client " + std::to_string(i + 1) + "\" Client.exe";

        if (CreateProcessA(NULL, const_cast<char*>(cmd.c_str()),
            NULL, NULL, FALSE, CREATE_NEW_CONSOLE, NULL, NULL, &si, &pi)) {
            CloseHandle(pi.hProcess);
            CloseHandle(pi.hThread);
            Sleep(300); // Небольшая задержка между запусками
        }
        else {
            std::cerr << "Failed to start client: " << GetLastError() << "\n";
        }
    }
}

void waitForClientsToFinish() {
    while (activeClients > 0) {
        Sleep(100);
    }
    std::cout << "All clients have disconnected.\n";
    // Выводим итоговое состояние файла
    std::cout << "Final file state:\n";
    printEmployees();
}

int main() {
    initializeFile();

    std::thread serverThread(runServer);
    Sleep(1000); // Даем серверу время на инициализацию

    int clientCount;
    std::cout << "Enter number of clients to start: ";
    std::cin >> clientCount;
    startClients(clientCount);
    waitForClientsToFinish();
    std::cout << "Server is running. Press Enter to stop...\n";
    std::cin.ignore();
    std::cin.get();

    serverRunning = false;

    // Принудительно закрываем именованный канал
    HANDLE hTempPipe = CreateFileA(
        "\\\\.\\pipe\\EmployeePipe",
        GENERIC_READ | GENERIC_WRITE,
        0,
        NULL,
        OPEN_EXISTING,
        0,
        NULL);
    if (hTempPipe != INVALID_HANDLE_VALUE) {
        CloseHandle(hTempPipe);
    }

    serverThread.join();


    return 0;
}
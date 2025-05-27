#include <windows.h>
#include <iostream>
#include <fstream>
#include <string>

constexpr int MSG_SIZE = 20;

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cerr << "Usage: sender.exe <filename>\n";
        return 1;
    }

    HANDLE hMutex = OpenMutex(MUTEX_ALL_ACCESS, FALSE, "FileMutex");
    HANDLE hMessageReady = OpenEvent(EVENT_ALL_ACCESS, FALSE, "MessageReady");
    HANDLE hMessageProcessed = OpenEvent(EVENT_ALL_ACCESS, FALSE, "MessageProcessed");

    if (!hMutex || !hMessageReady || !hMessageProcessed) {
        std::cerr << "Error opening sync objects. Is receiver running? Error: " << GetLastError() << "\n";
        return 1;
    }

    std::string fileName = argv[1];
    std::string cmd;

    while (true) {
        std::cout << "Enter command (send/exit): ";
        std::cin >> cmd;

        if (cmd == "exit") break;

        if (cmd == "send") {
            std::cin.ignore();
            std::string msg;
            std::cout << "Enter message (<20 chars): ";
            std::getline(std::cin, msg);

            if (msg.length() > MSG_SIZE) {
                std::cerr << "Message too long!\n";
                continue;
            }

            WaitForSingleObject(hMessageProcessed, INFINITE);
            WaitForSingleObject(hMutex, INFINITE);

            std::ofstream file(fileName, std::ios::binary | std::ios::trunc);
            char buffer[MSG_SIZE] = { 0 };
            msg.copy(buffer, msg.length());
            file.write(buffer, MSG_SIZE);
            file.close();

            ReleaseMutex(hMutex);
            SetEvent(hMessageReady);

            std::cout << "Message sent. Waiting for receiver...\n";
        }
    }

    CloseHandle(hMutex);
    CloseHandle(hMessageReady);
    CloseHandle(hMessageProcessed);
    return 0;
}
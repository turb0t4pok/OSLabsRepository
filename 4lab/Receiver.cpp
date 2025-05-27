#include <windows.h>
#include <iostream>
#include <fstream>
#include <string>

constexpr int MSG_SIZE = 20;

void LaunchSenderInNewWindow(const std::string& fileName) {
    STARTUPINFO si = { sizeof(si) };
    PROCESS_INFORMATION pi;
    std::string cmd = "cmd.exe /c start \"Sender Console\" sender.exe \"" + fileName + "\"";

    if (!CreateProcess(
        NULL,                  
        (LPSTR)cmd.c_str(), 
        NULL,          
        NULL,               
        FALSE,               
        CREATE_NEW_CONSOLE,    
        NULL,                
        NULL,                 
        &si,                   
        &pi                    
    )) {
        std::cerr << "Failed to launch sender. Error: " << GetLastError() << "\n";
        return;
    }

    CloseHandle(pi.hThread);
    CloseHandle(pi.hProcess);
}

int main() {
    HANDLE hMutex = CreateMutex(NULL, FALSE, "FileMutex");
    HANDLE hMessageReady = CreateEvent(NULL, FALSE, FALSE, "MessageReady");
    HANDLE hMessageProcessed = CreateEvent(NULL, FALSE, TRUE, "MessageProcessed");

    if (!hMutex || !hMessageReady || !hMessageProcessed) {
        std::cerr << "Error creating sync objects: " << GetLastError() << "\n";
        return 1;
    }

    std::string fileName;
    std::cout << "Enter filename: ";
    std::cin >> fileName;

    {
        std::ofstream file(fileName, std::ios::binary | std::ios::trunc);
        char empty[MSG_SIZE] = { 0 };
        file.write(empty, MSG_SIZE);
    }

    LaunchSenderInNewWindow(fileName);
    std::cout << "Sender launched in new window. Waiting for messages...\n";

    std::string cmd;
    while (true) {
        std::cout << "Enter command (read/exit): ";
        std::cin >> cmd;

        if (cmd == "exit") break;

        if (cmd == "read") {
            WaitForSingleObject(hMessageReady, INFINITE);
            WaitForSingleObject(hMutex, INFINITE);

            std::ifstream file(fileName, std::ios::binary);
            char buffer[MSG_SIZE + 1] = { 0 };
            file.read(buffer, MSG_SIZE);
            std::cout << "Received: " << buffer << "\n";
            file.close();

            std::ofstream clearFile(fileName, std::ios::binary | std::ios::trunc);
            char empty[MSG_SIZE] = { 0 };
            clearFile.write(empty, MSG_SIZE);
            clearFile.close();

            ReleaseMutex(hMutex);
            SetEvent(hMessageProcessed);
        }
    }

    CloseHandle(hMutex);
    CloseHandle(hMessageReady);
    CloseHandle(hMessageProcessed);
    return 0;
}
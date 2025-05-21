#include <windows.h>
#include <iostream>
#include <string>
#include <limits>

struct employee {
    int num;
    char name[10];
    double hours;
};

HANDLE connectToPipe() {
    for (int i = 0; i < 5; ++i) {
        HANDLE hPipe = CreateFileA(
            "\\\\.\\pipe\\EmployeePipe",
            GENERIC_READ | GENERIC_WRITE,
            0,
            NULL,
            OPEN_EXISTING,
            0,
            NULL);

        if (hPipe != INVALID_HANDLE_VALUE) {
            DWORD mode = PIPE_READMODE_MESSAGE;
            if (SetNamedPipeHandleState(hPipe, &mode, NULL, NULL)) {
                return hPipe;
            }
            CloseHandle(hPipe);
        }
        Sleep(500);
    }
    return INVALID_HANDLE_VALUE;
}

bool sendCommand(HANDLE hPipe, char cmd, int id) {
    DWORD bytesWritten;
    if (!WriteFile(hPipe, &cmd, sizeof(cmd), &bytesWritten, NULL) ||
        !WriteFile(hPipe, &id, sizeof(id), &bytesWritten, NULL)) {
        return false;
    }
    return true;
}

void modifyRecord(HANDLE hPipe) {
    int id;
    std::cout << "Enter employee ID: ";
    std::cin >> id;

    // Отправляем запрос на модификацию
    char op = 'W';
    DWORD bytesWritten;
    if (!WriteFile(hPipe, &op, sizeof(op), &bytesWritten, NULL) ||
        !WriteFile(hPipe, &id, sizeof(id), &bytesWritten, NULL)) {
        std::cout << "Error sending request to server\n";
        return;
    }

    // Получаем текущую запись
    employee emp;
    DWORD bytesRead;
    if (!ReadFile(hPipe, &emp, sizeof(emp), &bytesRead, NULL) || emp.num == 0) {
        std::cout << "Employee not found!\n";
        return;
    }

    // Показываем текущие данные
    std::cout << "\nCurrent record:\n";
    std::cout << "ID: " << emp.num << "\n";
    std::cout << "Name: " << emp.name << "\n";
    std::cout << "Hours: " << emp.hours << "\n\n";

    // Ввод новых данных
    employee newEmp = emp;
    std::cout << "Enter new name (current: " << emp.name << "): ";
    std::cin.ignore();
    std::cin.getline(newEmp.name, sizeof(newEmp.name));

    std::cout << "Enter new hours (current: " << emp.hours << "): ";
    std::cin >> newEmp.hours;

    // Подтверждение отправки
    std::cout << "\nPress Enter to send changes to server...";
    std::cin.ignore();
    std::cin.get();

    // Отправляем изменения
    if (!WriteFile(hPipe, &newEmp, sizeof(newEmp), &bytesWritten, NULL)) {
        std::cout << "Error sending changes to server\n";
        return;
    }

    // Получаем подтверждение от сервера
    char response[3] = { 0 };
    if (ReadFile(hPipe, response, sizeof(response), &bytesRead, NULL)) {
        std::cout << "Server response: " << response << "\n";
    }

    // Ожидание команды завершения доступа
    std::cout << "Press Enter to release the record...";
    std::cin.get();

    // Завершение доступа к записи
    char finish = 'F';
    WriteFile(hPipe, &finish, sizeof(finish), &bytesWritten, NULL);
}

void readRecord(HANDLE hPipe) {
    int id;
    std::cout << "Enter employee ID: ";
    std::cin >> id;

    // Отправка запроса на чтение
    char op = 'R';
    WriteFile(hPipe, &op, sizeof(op), NULL, NULL);
    WriteFile(hPipe, &id, sizeof(id), NULL, NULL);

    // Получение данных
    employee emp;
    DWORD bytesRead;
    if (!ReadFile(hPipe, &emp, sizeof(emp), &bytesRead, NULL) || emp.num == 0) {
        std::cout << "Employee not found!\n";
        return;
    }

    std::cout << "\nEmployee record:\n";
    std::cout << "ID: " << emp.num << "\nName: " << emp.name << "\nHours: " << emp.hours << "\n";

    // Ожидание команды завершения доступа
    std::cout << "Press Enter to release the record...";
    std::cin.ignore();
    std::cin.get();

    // Отправка сигнала о завершении доступа
    char finish = 'F';
    WriteFile(hPipe, &finish, sizeof(finish), NULL, NULL);
}

int main() {
    std::cout << "Client started. Connecting to server...\n";

    HANDLE hPipe = connectToPipe();
    if (hPipe == INVALID_HANDLE_VALUE) {
        std::cerr << "Failed to connect to server\n";
        system("pause");
        return 1;
    }

    // 2.1. Основной цикл клиента
    while (true) {
        std::cout << "\nMain menu:\n";
        std::cout << "1. Modify record\n";
        std::cout << "2. Read record\n";
        std::cout << "3. Exit\n";
        std::cout << "Choice: ";

        int choice;
        std::cin >> choice;

        switch (choice) {
        case 1:
            modifyRecord(hPipe);
            break;
        case 2:
            readRecord(hPipe);
            break;
        case 3:
            CloseHandle(hPipe);
            return 0;
        default:
            std::cout << "Invalid choice!\n";
            std::cin.clear();
            std::cin.ignore();
        }
    }
}
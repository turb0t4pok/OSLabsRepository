#include <windows.h>
#include <iostream>
#include <vector>
#include <ctime>
#include "function.h"

using namespace std;

int* shared_array = nullptr;
int array_size;
int marker_count;

vector<HANDLE> marker_threads;
vector<HANDLE> marker_events_continue;
vector<HANDLE> marker_events_stop;
vector<bool> thread_finished;
vector<vector<int>> marked_by_thread;

HANDLE start_event;
HANDLE all_blocked_event;
CRITICAL_SECTION cs;

volatile int blocked_threads = 0;

struct ThreadParams {
    int id;
};

DWORD WINAPI MarkerThread(LPVOID param) {
    int id = ((ThreadParams*)param)->id;
    srand(id);

    WaitForSingleObject(start_event, INFINITE);

    while (true) {
        int rnd = rand();
        int index = rnd % array_size;

        bool marked = false;

        EnterCriticalSection(&cs);
        if (canMarkIndex(shared_array, index)) {
            Sleep(5);
            shared_array[index] = id + 1;
            marked_by_thread[id].push_back(index);
            Sleep(5);
            marked = true;
        }
        LeaveCriticalSection(&cs);

        if (marked)
            continue;

        EnterCriticalSection(&cs);
        cout << "поток номер " << id + 1 << " не может заменить число. Количество занесений: "
            << marked_by_thread[id].size() << ", Индекс: " << index << endl;
        blocked_threads++;
        if (blocked_threads == marker_count) {
            SetEvent(all_blocked_event);
        }
        LeaveCriticalSection(&cs);

        // Ждём сигнал (или завершение)
        HANDLE wait_handles[2] = { marker_events_continue[id], marker_events_stop[id] };
        DWORD res = WaitForMultipleObjects(2, wait_handles, FALSE, INFINITE);

        if (res == WAIT_OBJECT_0 + 1) {
            // Завершение
            EnterCriticalSection(&cs);
            clearMarks(shared_array, marked_by_thread[id]);
            LeaveCriticalSection(&cs);
            return 0;
        }

        // Сброс блокировки и продолжение
        EnterCriticalSection(&cs);
        blocked_threads--;
        LeaveCriticalSection(&cs);
    }
    return 0;
}


int main() {
    setlocale(LC_ALL, "rus");
    InitializeCriticalSection(&cs);

    cout << "Введите размер массива: ";
    cin >> array_size;
    shared_array = new int[array_size] {};

    cout << "Введите количество потоков: ";
    cin >> marker_count;

    marker_events_continue.resize(marker_count);
    marker_events_stop.resize(marker_count);
    thread_finished.resize(marker_count, false);
    marked_by_thread.resize(marker_count);

    start_event = CreateEvent(NULL, TRUE, FALSE, NULL);
    all_blocked_event = CreateEvent(NULL, FALSE, FALSE, NULL);

    for (int i = 0; i < marker_count; ++i) {
        marker_events_continue[i] = CreateEvent(NULL, FALSE, FALSE, NULL);
        marker_events_stop[i] = CreateEvent(NULL, FALSE, FALSE, NULL);

        ThreadParams* params = new ThreadParams{ i };
        HANDLE hThread = CreateThread(NULL, 0, MarkerThread, params, 0, NULL);
        marker_threads.push_back(hThread);
    }

    SetEvent(start_event);

    while (true) {
        // Ждём, пока все потоки сообщат о невозможности продолжения
        WaitForSingleObject(all_blocked_event, INFINITE);

        // Вывод текущего состояния массива
        EnterCriticalSection(&cs);
        cout << "Массив: ";
        for (int i = 0; i < array_size; ++i)
            cout << shared_array[i] << " ";
        cout << endl;
        LeaveCriticalSection(&cs);

        // Запрос на завершение потока
        int terminate_id;
        while (true) {
            EnterCriticalSection(&cs);
            cout << "Введите номер потока для завершения (1.." << marker_count << "): ";
            cin >> terminate_id;
            LeaveCriticalSection(&cs);
            if (cin.fail()) {
                cin.clear();
                cin.ignore(INT_MAX, '\n');
                cout << "Ошибка ввода. попробуйте ещё" << endl;
                continue;
            }

            terminate_id -= 1;

            if (terminate_id < 0 || terminate_id >= marker_count) {
                cout << "Ошибка ввода. попробуйте ещё" << endl;
                continue;
            }

            if (thread_finished[terminate_id]) {
                cout << "Поток номер" << terminate_id + 1 << " завершён. Выберите другой" << endl;
                continue;
            }

            break;
        }

        // Завершаем указанный поток
        SetEvent(marker_events_stop[terminate_id]);
        WaitForSingleObject(marker_threads[terminate_id], INFINITE);
        CloseHandle(marker_threads[terminate_id]);
        thread_finished[terminate_id] = true;

        // Вывод состояния после завершения
        EnterCriticalSection(&cs);
        cout << "После завершения:\nМассив: ";
        for (int i = 0; i < array_size; ++i)
            cout << shared_array[i] << " ";
        cout << endl;
        LeaveCriticalSection(&cs);

        // Проверка, остались ли активные потоки
        int alive = 0;
        for (bool finished : thread_finished)
            if (!finished) alive++;

        if (alive == 0)
            break;

        // Разрешаем оставшимся потокам продолжить
        for (int i = 0; i < marker_count; ++i) {
            if (!thread_finished[i]) {
                SetEvent(marker_events_continue[i]);
            }
        }
    }


    DeleteCriticalSection(&cs);
    delete[] shared_array;

    for (HANDLE h : marker_events_continue) CloseHandle(h);
    for (HANDLE h : marker_events_stop) CloseHandle(h);
    CloseHandle(start_event);
    CloseHandle(all_blocked_event);

    return 0;
}

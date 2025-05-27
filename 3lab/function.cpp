#include "function.h"

bool canMarkIndex(int* arr, int index) {
    return arr[index] == 0;
}

void clearMarks(int* arr, const std::vector<int>& indices) {
    for (int idx : indices) {
        arr[idx] = 0;
    }
}

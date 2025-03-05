#include "array_processing.h"
#include <thread>
#include <chrono>
#include <limits>

using namespace std;

void findMinMax(const vector<int>& arr, int& minValue, int& maxValue) {
    minValue = numeric_limits<int>::max();
    maxValue = numeric_limits<int>::min();

    for (int val : arr) {
        if (val < minValue) minValue = val;
        if (val > maxValue) maxValue = val;
        this_thread::sleep_for(chrono::milliseconds(7));
    }
}

void computeAverage(const vector<int>& arr, double& averageValue) {
    long long sum = 0;
    for (int val : arr) {
        sum += val;
        this_thread::sleep_for(chrono::milliseconds(12));
    }
    averageValue = static_cast<double>(sum) / arr.size();
}

void replaceMinMaxWithAverage(vector<int>& arr, int minValue, int maxValue, double averageValue) {
    for (int& val : arr) {
        if (val == minValue || val == maxValue) {
            val = static_cast<int>(averageValue);
        }
    }
}

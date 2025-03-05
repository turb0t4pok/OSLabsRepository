#include <iostream>
#include <vector>
#include <thread>
#include <chrono>
#include <limits>
#include <mutex>

using namespace std;

int minValue, maxValue;
double averageValue;

void findMinMax(const vector<int>& arr) {
    minValue = numeric_limits<int>::max();
    maxValue = numeric_limits<int>::min();

    for (int val : arr) {
        if (val < minValue) minValue = val;
        if (val > maxValue) maxValue = val;
        this_thread::sleep_for(chrono::milliseconds(7));
    }

    cout << "Min: " << minValue << ", Max: " << maxValue << endl;
}

void computeAverage(const vector<int>& arr) {
    long long sum = 0;
    for (int val : arr) {
        sum += val;
        this_thread::sleep_for(chrono::milliseconds(12));
    }

    averageValue = static_cast<double>(sum) / arr.size();
    cout << "Average: " << averageValue << endl;
}

int main() {
    int n;
    cout << "Enter array size: ";
    cin >> n;

    vector<int> arr(n);
    cout << "Enter array elements: ";
    for (int& val : arr) {
        cin >> val;
    }

    thread tMinMax(findMinMax, cref(arr));
    thread tAverage(computeAverage, cref(arr));

    tMinMax.join();
    tAverage.join();

    for (int& val : arr) {
        if (val == minValue || val == maxValue) {
            val = static_cast<int>(averageValue);
        }
    }

    cout << "Modified array: ";
    for (int val : arr) {
        cout << val << " ";
    }
    cout << endl;

    return 0;
}

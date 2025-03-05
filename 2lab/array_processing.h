#pragma once
#include <vector>

void findMinMax(const std::vector<int>& arr, int& minValue, int& maxValue);
void computeAverage(const std::vector<int>& arr, double& averageValue);
void replaceMinMaxWithAverage(std::vector<int>& arr, int minValue, int maxValue, double averageValue);

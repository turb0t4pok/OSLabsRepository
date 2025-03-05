#include "array_processing.h"
#include <gtest/gtest.h>
#include <vector>

using namespace std;

TEST(FindMinMaxTest, CorrectMinMax) {
    vector<int> arr = { 5, 2, 8, 1, 9 };
    int minVal = 0, maxVal = 0;
    findMinMax(arr, minVal, maxVal);
    EXPECT_EQ(minVal, 1);
    EXPECT_EQ(maxVal, 9);
}

TEST(ComputeAverageTest, CorrectAverage) {
    vector<int> arr = { 4, 6, 8, 2 };
    double avg = 0;
    computeAverage(arr, avg);
    EXPECT_DOUBLE_EQ(avg, 5.0);
}

TEST(ReplaceMinMaxTest, ReplaceCorrectly) {
    vector<int> arr = { 5, 2, 8, 2, 9 };
    int minVal = 2, maxVal = 9;
    double avg = 6.0;
    replaceMinMaxWithAverage(arr, minVal, maxVal, avg);

    vector<int> expected = { 5, 6, 8, 6, 6 };
    EXPECT_EQ(arr, expected);
}

TEST(FullIntegrationTest, CompleteScenario) {
    vector<int> arr = { 10, 20, 30, 5, 100 };
    int minVal = 0, maxVal = 0;
    double avg = 0;

    thread t1(findMinMax, cref(arr), ref(minVal), ref(maxVal));
    thread t2(computeAverage, cref(arr), ref(avg));

    t1.join();
    t2.join();

    replaceMinMaxWithAverage(arr, minVal, maxVal, avg);

    EXPECT_EQ(minVal, 5);
    EXPECT_EQ(maxVal, 100);
    EXPECT_DOUBLE_EQ(avg, 33.0);

    vector<int> expected = { 10, 20, 30, 33, 33 };
    EXPECT_EQ(arr, expected);
}

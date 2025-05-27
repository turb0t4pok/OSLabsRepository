#define CATCH_CONFIG_MAIN
#include "catch.hpp"
#include "function.h"


TEST_CASE("canMarkIndex", "[canMarkIndex]") {
    int arr[] = { 0, 1, 2 };
    REQUIRE(canMarkIndex(arr, 0));
    REQUIRE_FALSE(canMarkIndex(arr, 1));
}

TEST_CASE("clearMarks", "[clearMarks]") {
    int arr[] = { 5, 6, 7, 8 };
    std::vector<int> marks = { 1, 3 };
    clearMarks(arr, marks);
    REQUIRE(arr[0] == 5);
    REQUIRE(arr[1] == 0);
    REQUIRE(arr[2] == 7);
    REQUIRE(arr[3] == 0);
}

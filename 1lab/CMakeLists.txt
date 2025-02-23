cmake_minimum_required(VERSION 3.10)
project(lab1)

set(CMAKE_CXX_STANDARD 20) 
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_CXX_EXTENSIONS OFF)  

include_directories(${CMAKE_SOURCE_DIR})
add_definitions(-UUNICODE -U_UNICODE)

add_executable(Creator creator.cpp)
add_executable(Main main.cpp)
add_executable(Reporter reporter.cpp)

set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/refs/tags/v1.14.0.zip
)
FetchContent_MakeAvailable(googletest)

enable_testing()

add_executable(tests test_main.cpp test_creator.cpp test_reporter.cpp)

target_link_libraries(tests PRIVATE gtest gtest_main)

add_dependencies(tests Creator Reporter)

add_test(NAME tests COMMAND tests)

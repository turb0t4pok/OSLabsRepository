cmake_minimum_required(VERSION 3.14)
project(EmployeeSystem)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Главные исходники
add_executable(Server Server.cpp)
add_executable(Client Client.cpp)

# Тесты
include(FetchContent)
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/refs/tags/v1.14.0.zip
)
FetchContent_MakeAvailable(googletest)

enable_testing()
add_executable(EmployeeTests EmployeeTests.cpp EmployeeUtils.cpp)
target_include_directories(EmployeeTests PRIVATE ${PROJECT_SOURCE_DIR})
target_link_libraries(EmployeeTests gtest gtest_main)
include(GoogleTest)
gtest_discover_tests(EmployeeTests)

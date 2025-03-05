cmake_minimum_required(VERSION 3.14)
project(array_processing)

set(CMAKE_CXX_STANDARD 17)

# Главный исполняемый файл
add_executable(array_main main.cpp array_processing.cpp)

# Подключаем GoogleTest через FetchContent
include(FetchContent)

set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)  # ключевая строка для Windows
FetchContent_Declare(
    googletest
    URL https://github.com/google/googletest/archive/refs/heads/main.zip
)

# Загружаем и инициализируем
FetchContent_MakeAvailable(googletest)

# Тесты
enable_testing()

add_executable(array_tests array_processing_test.cpp array_processing.cpp)
target_link_libraries(array_tests gtest_main)
include(GoogleTest)
gtest_discover_tests(array_tests)

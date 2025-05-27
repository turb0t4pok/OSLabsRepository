cmake_minimum_required(VERSION 3.10)

project(MyProject)

set(CMAKE_CXX_STANDARD 98) 
set(CMAKE_CXX_STANDARD_REQUIRED True)


add_executable(main main.cpp function.cpp)


add_executable(test_runner unit.cpp function.cpp)
target_link_libraries(test_runner)
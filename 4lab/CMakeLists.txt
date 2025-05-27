cmake_minimum_required(VERSION 3.10)
project(SingleMessageIPC)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)

add_executable(receiver receiver.cpp)
add_executable(sender sender.cpp)
add_executable(unit unit.cpp)


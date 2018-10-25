cmake_minimum_required(VERSION 3.10)
project(Homework_1 C)

set(CMAKE_C_STANDARD 99)
set(CMAKE_BUILD_TYPE Debug)

add_library(common c_string.c file.c project_util.c)
add_executable(test_o main.c)
target_link_libraries(test_o common)
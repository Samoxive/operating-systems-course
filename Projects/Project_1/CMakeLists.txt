cmake_minimum_required(VERSION 3.10)
project(Homework_1 C)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -pedantic -Wextra")

add_library(common c_string.c file.c project_util.c)
add_executable(test_o main.c)
add_executable(psearch_normal psearch_normal.c)
add_executable(psearch1a psearch1a.c)
add_executable(psearch1b psearch1b.c)
add_executable(psearch1c psearch1c.c)
add_executable(psearch2a psearch2a.c)
add_executable(psearch2aslave psearch2aslave.c)
add_executable(psearch2b psearch2b.c)
add_executable(psearch2bslave psearch2bslave.c)
add_executable(psearch2c psearch2c.c)
add_executable(psearch2cslave psearch2cslave.c)
add_executable(psearch2d psearch2d.c)
add_executable(psearch2dslave psearch2dslave.c)
add_executable(psearch3 psearch3.c)
target_link_libraries(test_o common)
target_link_libraries(psearch_normal common)
target_link_libraries(psearch1a common)
target_link_libraries(psearch1b common pthread rt)
target_link_libraries(psearch1c common)
target_link_libraries(psearch2a common pthread rt)
target_link_libraries(psearch2aslave common pthread rt)
target_link_libraries(psearch2b common pthread rt)
target_link_libraries(psearch2bslave common pthread rt)
target_link_libraries(psearch2c common pthread rt)
target_link_libraries(psearch2cslave common pthread rt)
target_link_libraries(psearch2d common pthread rt)
target_link_libraries(psearch2dslave common pthread rt)
target_link_libraries(psearch3 common pthread rt)

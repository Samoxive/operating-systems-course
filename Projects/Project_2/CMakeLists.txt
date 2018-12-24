cmake_minimum_required(VERSION 3.10)
project(Project_2 C)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -pedantic -Wextra -g")

add_executable(main main.c)
target_link_libraries(main pthread)

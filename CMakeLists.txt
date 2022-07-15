cmake_minimum_required(VERSION 3.16)
project(minishell C)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_COMPILER /usr/bin/gcc)

include_directories(/usr/include/readline)
include_directories(/usr/lib64/libtinfo)
file(GLOB SOURCES "./*.c")

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -lreadline -ltinfo")
add_executable(minishell src/main.c src/minishell.h src/minishell.c src/job.h src/process.h src/job.c)
target_link_libraries(minishell readline tinfo)

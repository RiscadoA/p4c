cmake_minimum_required(VERSION 3.17)
project(p4c-compiler)

set(SOURCES
    "src/p4c/entry.c"
    "src/p4c/lexer.c"
    "src/p4c/parser.c"
)

add_executable(p4c ${SOURCES})
target_include_directories(p4c PRIVATE "src/")

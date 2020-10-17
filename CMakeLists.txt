cmake_minimum_required(VERSION 3.17)
project(p4c-compiler)

set(SOURCES
    "src/p4c/entry.cpp"
    "src/p4c/token.cpp"
)

add_executable(p4c ${SOURCES})
target_include_directories(p4c PRIVATE "src/")

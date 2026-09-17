cmake_minimum_required(VERSION 3.16)
project(regex_to_nfa C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

include_directories(src)


add_executable(regex_to_nfa
    ./src/main.c
    ./src/nfa.c
    ./src/dfa.c
    ./src/regex.c
    ./src/conjunto.c
)


enable_testing()
add_executable(ejecutar_pruebas
    ./src/test/test_dfa.c
    ./src/dfa.c
    ./src/nfa.c
    ./src/regex.c
    ./src/conjunto.c
)

add_test(NAME PruebasDFA COMMAND ejecutar_pruebas)


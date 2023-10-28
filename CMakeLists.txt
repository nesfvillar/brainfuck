cmake_minimum_required(VERSION 3.0.0)
project(brainfuck VERSION 0.1.0 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 23)
add_compile_options(/W4 /WX)

include(CTest)
enable_testing()

add_executable(brainfuck)
target_sources(brainfuck
    PRIVATE
    src/brainfuck.cpp)
target_include_directories(brainfuck
    PRIVATE
    inc)

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)

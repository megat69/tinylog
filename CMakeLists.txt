cmake_minimum_required(VERSION 3.14)
project(TinyLog LANGUAGES CXX)

string(LENGTH "${CMAKE_SOURCE_DIR}/" SOURCE_PATH_SIZE)
add_compile_definitions("SOURCE_PATH_SIZE=${SOURCE_PATH_SIZE}")

add_executable(test_tinylog test/test_tinylog.cpp)

target_include_directories(test_tinylog PUBLIC src/tinylog)

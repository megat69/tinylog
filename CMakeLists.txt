cmake_minimum_required(VERSION 3.14)
project(TinyLog LANGUAGES CXX)

add_executable(test_tinylog test/test_tinylog.cpp)

target_include_directories(test_tinylog PUBLIC src/tinylog)

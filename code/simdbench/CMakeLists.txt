cmake_minimum_required(VERSION 4.0)
project(pdemo LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 26)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(benchmark REQUIRED)

add_executable(bench1 bench1.cpp external.cpp)

target_link_libraries(bench1 benchmark::benchmark)

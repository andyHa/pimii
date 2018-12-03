cmake_minimum_required(VERSION 3.12)
project(pimii-tests)

set(CMAKE_CXX_STANDARD 17)

add_executable(pimii-tests tests-main.cpp ObjectPointerSpec.cpp)
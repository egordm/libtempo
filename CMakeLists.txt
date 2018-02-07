cmake_minimum_required(VERSION 3.9)
project(${MAIN_PROJECT_NAME})

set(CMAKE_CXX_STANDARD 11)

enable_testing()

add_subdirectory(main)
add_subdirectory(test)
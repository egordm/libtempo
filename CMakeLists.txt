cmake_minimum_required(VERSION 3.9)
project(${MAIN_PROJECT_NAME})

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)
set(LIB_DIR "${CMAKE_CURRENT_SOURCE_DIR}/libs")

enable_testing()

add_subdirectory(tempogram)
add_subdirectory(main)
add_subdirectory(test)
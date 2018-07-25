cmake_minimum_required(VERSION 3.9)
project(tempogram_tempo_estimation)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)
set(LIB_DIR "${CMAKE_CURRENT_SOURCE_DIR}/libs")

enable_testing()

find_package(BLAS REQUIRED)
include_directories(${BLAS_INCLUDE_DIR})
find_package(LAPACK REQUIRED)
include_directories(${LAPACK_INCLUDE_DIR})
find_package(Armadillo REQUIRED)
include_directories(${ARMADILLO_INCLUDE_DIR})
include_directories(${SIGPACK_INCLUDE_DIR})
message(STATUS ${SIGPACK_INCLUDE_DIR})

add_subdirectory(tempogram)
add_subdirectory(main)
add_subdirectory(py_library)
add_subdirectory(test)
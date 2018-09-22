cmake_minimum_required(VERSION 3.9)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)

# Libraries
set(LIB_DIR "${CMAKE_CURRENT_SOURCE_DIR}/libs")
set(MINIMP3_INCLUDE_DIR "${LIB_DIR}/minimp3")
set(ARMADILLO_LIBRARY "${LIB_DIR}/armadillo")
set(ARMADILLO_INCLUDE_DIR "${LIB_DIR}/armadillo/include")
set(SIGPACK_INCLUDE_DIR "${LIB_DIR}/sigpack")

if (WIN32)
    set(CMAKE_SHARED_LIBRARY_PREFIX "")
endif ()

#enable_testing()

function(defineLibIncludes)
    get_property(tmp_include_dirs DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY INCLUDE_DIRECTORIES)
    set(${PROJECT_NAME}_INCLUDE_DIRS ${PROJECT_SOURCE_DIR} ${tmp_include_dirs}
            CACHE INTERNAL "${PROJECT_NAME}: Include Directories" FORCE)
endfunction()

add_subdirectory(audio_io)
add_subdirectory(libtempo)
add_subdirectory(libtempo_py)
add_subdirectory(main)
#add_subdirectory(test)
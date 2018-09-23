set(BUILD_GMOCK 1)
add_subdirectory(${GOOGLETEST_ROOT} googletest)

include_directories(${gtest_SOURCE_DIR}/include ${gtest_SOURCE_DIR})
include_directories(${gmock_SOURCE_DIR}/include ${gmock_SOURCE_DIR})

macro(setTestName)
    string(REPLACE "/" ";" CUR_DIRECTORY_TOKENS ${CMAKE_CURRENT_LIST_DIR})
    LIST(REVERSE CUR_DIRECTORY_TOKENS)
    LIST(GET CUR_DIRECTORY_TOKENS 0 PROJECT_NAME)
endmacro()

set(GOOGLETEST_LIBRARIES gtest gmock_main)

add_subdirectory(test_main)
add_subdirectory(test_libtempo)

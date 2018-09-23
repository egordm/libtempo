set(BUILD_GMOCK 1)
add_subdirectory(${GOOGLETEST_ROOT} googletest)

include_directories(${gtest_SOURCE_DIR}/include ${gtest_SOURCE_DIR})
include_directories(${gmock_SOURCE_DIR}/include ${gmock_SOURCE_DIR})

set(GOOGLETEST_LIBRARIES gtest gmock_main)

add_subdirectory(test_main)

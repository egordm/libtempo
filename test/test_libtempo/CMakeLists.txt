file(GLOB SRCS *.cpp)

setTestName()

include_directories(${libtempo_INCLUDE_DIRS})
add_executable(${PROJECT_NAME} ${SRCS})
target_link_libraries(${PROJECT_NAME} ${GOOGLETEST_LIBRARIES} libtempo)

add_test(NAME ${PROJECT_NAME} COMMAND ${PROJECT_NAME})

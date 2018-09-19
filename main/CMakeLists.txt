project(main C CXX)

file(GLOB SOURCE_FILES
        "*.h"
        "*.cpp"
        "*.hpp"
        )

include_directories(${tempo_estimation_INCLUDE_DIRS} ${audio_io_INCLUDE_DIRS} ../libs/args ../libs/json/single_include)

add_executable(${PROJECT_NAME} ${SOURCE_FILES})

if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
    set(COMPILER_LIBS stdc++fs)
else ()
endif ()

target_link_libraries(${PROJECT_NAME} tempo_estimation audio_io ${COMPILER_LIBS})

install(TARGETS ${PROJECT_NAME}
        RUNTIME DESTINATION bin
        LIBRARY DESTINATION lib
        ARCHIVE DESTINATION lib)
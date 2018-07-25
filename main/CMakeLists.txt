project(main C CXX)

file(GLOB SOURCE_FILES
        "*.h"
        "*.cpp"
        "*.hpp"
        )

include_directories(${tempogram_INCLUDE_DIRS})

add_executable(${PROJECT_NAME} ${SOURCE_FILES})

target_link_libraries(${PROJECT_NAME} tempogram)

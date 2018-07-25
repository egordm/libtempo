file(GLOB SOURCE_FILES
        "*.h"
        "*.cpp"
        "*.hpp"
        )

include_directories(${ARMADILLO_INCLUDE_DIR})

add_executable(main ${SOURCE_FILES})

target_link_libraries(main tempogram)

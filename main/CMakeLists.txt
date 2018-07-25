file(GLOB SOURCE_FILES
        "*.h"
        "*.cpp"
        "*.hpp"
        )

add_executable(main ${SOURCE_FILES})

target_link_libraries(main tempogram)

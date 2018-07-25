file(GLOB SOURCE_FILES
        "*.h"
        "*.cpp"
        "*.hpp"
        )

add_library(tempogram ${SOURCE_FILES})

target_link_libraries(tempogram ${ARMADILLO_LIBRARIES})
file(GLOB SOURCE_FILES
        "*.h"
        "*.cpp"
        "*.hpp"
        )

find_package (Eigen3 3.3 REQUIRED NO_MODULE)

add_library(tempogram ${SOURCE_FILES})

target_link_libraries (tempogram Eigen3::Eigen)
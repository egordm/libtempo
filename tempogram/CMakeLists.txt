file(GLOB SOURCE_FILES
        "*.h"
        "*.cpp"
        "*.hpp"
        )

find_package(Armadillo REQUIRED)
include_directories(${ARMADILLO_INCLUDE_DIRS})

include(PythonSetup)
include_directories(${PYTHON_INCLUDE_DIR})
find_package(pybind11 REQUIRED)
include_directories(${pybind11_INCLUDE_DIRS})

add_library(tempogram ${SOURCE_FILES})

target_link_libraries(tempogram ${ARMADILLO_LIBRARIES})
file(GLOB SOURCE_FILES
        "*.h"
        "*.cpp"
        "*.hpp"
        )
#
#include(PythonSetup)
#include_directories(${PYTHON_INCLUDE_DIR})

add_subdirectory(../../libs/pybind11 "${CMAKE_CURRENT_BINARY_DIR}/pybind11")

pybind11_add_module(py_library ${SOURCE_FILES})

target_link_libraries(py_library PRIVATE tempogram)

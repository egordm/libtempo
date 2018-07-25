project(tempo_estimation_py C CXX)

file(GLOB SOURCE_FILES
        "*.h"
        "*.cpp"
        "*.hpp"
        )

add_subdirectory(../../libs/pybind11 "${CMAKE_CURRENT_BINARY_DIR}/pybind11")

include_directories(${tempogram_INCLUDE_DIRS})

pybind11_add_module(${PROJECT_NAME} ${SOURCE_FILES})

target_link_libraries(${PROJECT_NAME} PRIVATE tempogram)

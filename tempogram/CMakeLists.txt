project(tempogram C CXX)

file(GLOB SOURCE_FILES
        "*.h"
        "*.cpp"
        "*.hpp"
        )

find_package(LibSndFile REQUIRED)
include_directories(${LibSndFile_INCLUDE_DIR})

find_package(Armadillo REQUIRED)
include_directories(${ARMADILLO_INCLUDE_DIR})
include_directories(${SIGPACK_INCLUDE_DIR})

add_library(${PROJECT_NAME} STATIC ${SOURCE_FILES})

target_link_libraries(${PROJECT_NAME} PUBLIC ${ARMADILLO_LIBRARIES} ${LIBSNDFILE_LIBRARIES})

set(${PROJECT_NAME}_INCLUDE_DIRS ${PROJECT_SOURCE_DIR}
        ${LibSndFile_INCLUDE_DIR}
        ${BLAS_INCLUDE_DIR}
        ${ARMADILLO_INCLUDE_DIR}
        ${SIGPACK_INCLUDE_DIR}
        CACHE INTERNAL "${PROJECT_NAME}: Include Directories" FORCE)

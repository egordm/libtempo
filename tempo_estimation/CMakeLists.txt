project(tempo_estimation C CXX)

file(GLOB SOURCE_FILES
        "*.h"
        "*.cpp"
        "*.hpp"
        )

find_package(LibSndFile REQUIRED)
include_directories(${LIBSNDFILE_INCLUDE_DIRS})


find_package(FFTW REQUIRED)
include_directories(${FFTW_INCLUDE_DIRS})
add_definitions(-DHAVE_FFTW)
message(STATUS ${FFTW_LIBRARIES})

find_package(Armadillo REQUIRED)
include_directories(${ARMADILLO_INCLUDE_DIR})
include_directories(${SIGPACK_INCLUDE_DIR})

add_library(${PROJECT_NAME} ${SOURCE_FILES})

target_link_libraries(${PROJECT_NAME} PUBLIC ${ARMADILLO_LIBRARIES} ${LIBSNDFILE_LIBRARIES} ${FFTW_LIBRARIES})

set(${PROJECT_NAME}_INCLUDE_DIRS ${PROJECT_SOURCE_DIR}
        ${FFTW_INCLUDE_DIRS}
        ${LIBSNDFILE_INCLUDE_DIRS}
        ${BLAS_INCLUDE_DIR}
        ${ARMADILLO_INCLUDE_DIR}
        ${SIGPACK_INCLUDE_DIR}
        CACHE INTERNAL "${PROJECT_NAME}: Include Directories" FORCE)

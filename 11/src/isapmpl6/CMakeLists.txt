cmake_minimum_required(VERSION 2.8)

set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x -Wl,-rpath=./")
include_directories (${CMAKE_CURRENT_SOURCE_DIR}/../include ${CMAKE_CURRENT_SOURCE_DIR}/../../../common/src/include)

if(False)
    find_package(OpenMP)
    if(NOT TARGET OpenMP::OpenMP_CXX AND (OpenMP_CXX_FOUND OR OPENMP_FOUND))
        set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
        set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${OpenMP_EXE_LINKER_FLAGS}")
    endif()
endif()
# Configure built shared libraries in top-level lib directory
set (CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/../../lib_isap6)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/opencv/include)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/ncnn/src)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/ncnn/build/src)
#include_directories(/home/isap/testlib/sdk/opencv/include)

link_directories(${CMAKE_CURRENT_SOURCE_DIR}/opencv/lib)
link_directories(${CMAKE_CURRENT_SOURCE_DIR}/ncnn/build/src)
#link_directories(/home/isap/testlib/sdk/opencv/lib)
link_directories(${CMAKE_CURRENT_SOURCE_DIR}/../../lib_isap6)
link_directories(${CMAKE_CURRENT_SOURCE_DIR})
# Build the shared libraries
add_library (frvt_11_isap_001 SHARED nullimplfrvt11.cpp FaceDetector.cpp FaceDetector.h)
target_link_libraries(frvt_11_isap_001 kenxnet)
target_link_libraries(frvt_11_isap_001 "-Wl,--whole-archive" ncnn "-Wl,--no-whole-archive")
target_link_libraries(frvt_11_isap_001 opencv_calib3d opencv_highgui opencv_objdetect opencv_imgproc opencv_core IlmImf libjasper libjpeg libpng libtiff)

if(OpenMP_CXX_FOUND)
    if(NCNN_CMAKE_VERBOSE)
        message("Building with OpenMP")
    endif()
    target_link_libraries(FaceDetector PUBLIC OpenMP::OpenMP_CXX)
endif()

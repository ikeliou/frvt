cmake_minimum_required(VERSION 2.8)

set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x -Wl,-rpath=./")
include_directories (${CMAKE_CURRENT_SOURCE_DIR}/../include ${CMAKE_CURRENT_SOURCE_DIR}/../../../common/src/include)

# Configure built shared libraries in top-level lib directory
set (CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/../../lib_isap6)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/opencv/include)
#include_directories(/home/isap/testlib/sdk/opencv/include)

link_directories(${CMAKE_CURRENT_SOURCE_DIR}/opencv/lib)
#link_directories(/home/isap/testlib/sdk/opencv/lib)
link_directories(${CMAKE_CURRENT_SOURCE_DIR}/../../lib_isap6)
# Build the shared libraries
add_library (frvt_11_isap_001 SHARED nullimplfrvt11.cpp)
target_link_libraries(frvt_11_isap_001 kenxnet)
target_link_libraries(frvt_11_isap_001 opencv_calib3d opencv_highgui opencv_objdetect opencv_imgproc opencv_core IlmImf libjasper libjpeg libpng libtiff)

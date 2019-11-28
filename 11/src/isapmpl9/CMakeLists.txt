cmake_minimum_required(VERSION 2.8)

set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIC -std=c++0x -Wl,-rpath=./")
include_directories (${CMAKE_CURRENT_SOURCE_DIR}/../include ${CMAKE_CURRENT_SOURCE_DIR}/../../../common/src/include)
if($ENV{debug})
    message("debug")
    add_definitions(-Ddebug=1)
endif()
# Configure built shared libraries in top-level lib directory
set (CMAKE_LIBRARY_OUTPUT_DIRECTORY $ENV{out})

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/opencv/include)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/ncnn/src)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/ncnn/build/src)

link_directories(${CMAKE_CURRENT_SOURCE_DIR}/opencv/lib)
#link_directories(${CMAKE_CURRENT_SOURCE_DIR}/ncnn/build/src)
link_directories($ENV{out})
# Build the shared libraries
add_library (frvt_11_isap_001 SHARED nullimplfrvt11.cpp)
target_link_libraries(frvt_11_isap_001 kenxnet)
target_link_libraries(frvt_11_isap_001 "-Wl,--whole-archive" /home/isap/ncnn/build/src/libncnn.a "-Wl,--no-whole-archive")
target_link_libraries(frvt_11_isap_001 ${CMAKE_CURRENT_SOURCE_DIR}/libretinaface_ncnn_lib.a)
target_link_libraries(frvt_11_isap_001 ${CMAKE_CURRENT_SOURCE_DIR}/liblffd_api.a)
target_link_libraries(frvt_11_isap_001 opencv_imgproc opencv_core IlmImf libjasper libjpeg libpng libtiff)


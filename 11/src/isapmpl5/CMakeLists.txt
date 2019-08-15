cmake_minimum_required(VERSION 2.8)

set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x -Wl,-rpath=./")
include_directories (${CMAKE_CURRENT_SOURCE_DIR}/../include ${CMAKE_CURRENT_SOURCE_DIR}/../../../common/src/include)

# Configure built shared libraries in top-level lib directory
set (CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/../../lib)

include_directories(/root/testlib/sdk/opencv/include)

link_directories(/root/testlib/sdk/opencv/lib)
#link_directories(/root/testfr)
link_directories(/root/frvt2019/11/lib)

# Build the shared libraries
add_library (frvt_11_isap_001 SHARED nullimplfrvt11.cpp)
#target_link_libraries(frvt_11_isap_001 /root/testfr/libkenxnet.so)
#target_link_libraries(frvt_11_isap_001 /usr/local/lib/libprotobuf.a)
target_link_libraries(frvt_11_isap_001 /root/frvt2019/11/lib/libkenxnet.so)
#target_link_libraries(frvt_11_isap_001 /root/tensorflow/tensorflow/contrib/makefile/gen/protobuf/lib/libprotobuf.a)
target_link_libraries(frvt_11_isap_001 opencv_calib3d opencv_highgui opencv_objdetect opencv_imgproc opencv_core IlmImf libjasper libjpeg libpng libtiff)
#target_link_libraries(frvt_11_isap_001 tensorflow_cc tensorflow_framework)

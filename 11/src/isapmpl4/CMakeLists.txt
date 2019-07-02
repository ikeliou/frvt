cmake_minimum_required(VERSION 2.8)

set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x -Wl,-rpath=./ -lpthread -lrt -lz -ldl -lidn -lm -lstdc++ -lssl -lcrypto")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DCPU_ONLY=1")
#set(CMAKE_CXX_STANDARD 11)
#set(CMAKE_CXX_STANDARD_REQUIRED TRUE)
include_directories (${CMAKE_CURRENT_SOURCE_DIR}/../include ${CMAKE_CURRENT_SOURCE_DIR}/../../../common/src/include)

# Configure built shared libraries in top-level lib directory
set (CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/../../lib)

#find_package(OpenCV REQUIRED)

#message(STATUS "OpenCV library status:")
#message(STATUS "    version: ${OpenCV_VERSION}")
#message(STATUS "    libraries: ${OpenCV_LIBS}")
#message(STATUS "    include path: ${OpenCV_INCLUDE_DIRS}")
#include_directories(${OpenCV_INCLUDE_DIRS})
include_directories(/root/mini-caffe/include)
include_directories(/root/testlib)
include_directories(/root/testlib/sdk/opencv/include)

link_directories(/root/mini-caffe/build)
link_directories(/root/testlib/sdk/opencv/lib)
link_directories(/root/testlib/sdk/mkl/lib)

#link_directories(${tensorflow_path}/lib)
# Build the shared libraries
add_library (frvt_11_isap_001 SHARED nullimplfrvt11.cpp MTCNN.cpp)
target_link_libraries(frvt_11_isap_001 /root/testlib/sdk/libFaceRecognition.a)
target_link_libraries(frvt_11_isap_001 /root/testlib/data/libiSapFrSDK.so)
target_link_libraries(frvt_11_isap_001 opencv_calib3d opencv_highgui opencv_objdetect opencv_imgproc opencv_core IlmImf libjasper libjpeg libpng libtiff)
target_link_libraries(frvt_11_isap_001 mkl_intel_lp64 mkl_sequential mkl_core)
target_link_libraries(frvt_11_isap_001 ${OpenCV_LIBS} "-static")
target_link_libraries(frvt_11_isap_001 caffe)
#target_link_libraries(frvt_11_isap_001 protobuf)

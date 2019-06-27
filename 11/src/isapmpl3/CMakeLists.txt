cmake_minimum_required(VERSION 2.8)

set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -Wall")

#set(CMAKE_CXX_STANDARD 11)
#set(CMAKE_CXX_STANDARD_REQUIRED TRUE)
include_directories (${CMAKE_CURRENT_SOURCE_DIR}/../include ${CMAKE_CURRENT_SOURCE_DIR}/../../../common/src/include)

# Configure built shared libraries in top-level lib directory
set (CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/../../lib)
set(tensorflow_path /root/tensorflow)

find_package(OpenCV REQUIRED)

message(STATUS "OpenCV library status:")
message(STATUS "    version: ${OpenCV_VERSION}")
message(STATUS "    libraries: ${OpenCV_LIBS}")
message(STATUS "    include path: ${OpenCV_INCLUDE_DIRS}")
include_directories(${OpenCV_INCLUDE_DIRS})
include_directories(${tensorflow_path})
link_directories(${tensorflow_path}/bazel-bin/tensorflow)
# Build the shared libraries
add_library (frvt_11_isap_001 SHARED nullimplfrvt11.cpp comm_lib.cpp  mtcnn.hpp  tensorflow_mtcnn.cpp utils.cpp)
#add_executable(frvt_11_isap_001 test.cpp comm_lib.cpp  mtcnn.hpp  tensorflow_mtcnn.cpp utils.cpp)
#target_link_libraries(frvt_11_isap_001 ${CMAKE_CURRENT_SOURCE_DIR}/libdlib.a)
#target_link_libraries(frvt_11_isap_001 /usr/local/lib64/libopencv_dnn.a)
target_link_libraries(frvt_11_isap_001 tensorflow_cc tensorflow_framework)
#target_link_libraries (frvt_11_isap_001 ${OpenCV_LIBS} "-static")
target_link_libraries (frvt_11_isap_001 ${OpenCV_LIBS} "-static")

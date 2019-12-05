#!/bin/bash

root=$(pwd)

echo "Attempting to build null implementation" 
cd src/isapmpl8
#make test.so
#mv test.so ../../lib/libfrvt_11_isap_001.so
rm -rf build; mkdir -p build; cd build
#cmake ../ dev/null; make
cmake ../ ; make

cd $root

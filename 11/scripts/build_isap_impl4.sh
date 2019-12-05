#!/bin/bash

root=$(pwd)

echo "Attempting to build null implementation" 
cd src/isapmpl4
rm -rf build; mkdir -p build; cd build
#cmake ../ > /dev/null; make
cmake ../ ; make
cd $root

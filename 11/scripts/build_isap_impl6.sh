#!/bin/bash

out=`readlink -f ./lib_isap6`
if [ "$1" != "" ]; then
    out=`readlink -f $1`
    echo $out
fi
touch "$out"/../isap6
export out
if [ "$2" != "" ]; then
    debug=1
    export debug
    echo $debug
fi
echo "Attempting to build null implementation" 
cd src/isapmpl6
#make test.so
#mv test.so ../../lib/libfrvt_11_isap_001.so
rm -rf build; mkdir -p build; cd build
#cmake ../ dev/null; make
cmake ../ ; make
cd -

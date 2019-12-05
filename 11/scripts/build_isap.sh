#!/bin/bash
code=$1
out=`readlink -f ./lib_isap$code`
if [ "$2" != "" ]; then
    out=`readlink -f $2`
    echo $out
fi
touch "$out"/../isap"$code"
export out
if [ "$3" != "" ]; then
    debug=1
    export debug
fi
echo $code $out $debug
echo "Attempting to build null implementation" 
cd src/isapmpl"$code"
#make test.so
#mv test.so ../../lib/libfrvt_11_isap_001.so
rm -rf build; mkdir -p build; cd build
#cmake ../ dev/null; make
cmake ../ ; make
cd -

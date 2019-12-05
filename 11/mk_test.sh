#!/bin/bash
code=$1
model=$2
mkdir -p test/$model/bin
mkdir -p test/$model/config/models
mkdir -p test/$model/lib
ln -s ../../../../config_isap5/models/fc.db test/$model/config/models/fc.db
ln -s ../../../lib_isap5/libtensorflow_cc.so test/$model/lib/libtensorflow_cc.so
ln -s ../../../lib_isap5/libtensorflow_framework.so.1 test/$model/lib/libtensorflow_framework.so
ln -s ../../../lib_isap5/libtensorflow_framework.so.1 test/$model/lib/libtensorflow_framework.so.1
path=model_isap/kennet/$model
if [ -e $path/fr.db ];then
    ln -sf ../../../../$path/fr.db test/$model/config/models/fr.db
fi
if [ -e $path/libkenxnet.so ];then
    ln -sf ../../../$path/libkenxnet.so test/$model/lib/libkenxnet.so
fi

#custom
if [ $code == 6 ];then
    mkdir -p test/$model/config/model
    ln -sf ../../../../config_isap6/model/face.bin test/$model/config/model/face.bin
    ln -sf ../../../../config_isap6/model/face.param test/$model/config/model/face.param
fi
if [ $code == 7 ];then
    mkdir -p test/$model/config/model
    ln -sf ../../../config_isap7/retina.bin test/$model/config/retina.bin
    ln -sf ../../../config_isap7/retina.param test/$model/config/retina.param
fi
if [ $code == 9 ] || [ $code == 10 ];then
    ar=(config_isap$code/*)
    for filename in "${ar[@]}"; do
        extension="${filename##*.}"
        file=${filename##*/}
        if [ $extension == "bin" ] || [ $extension == "param" ];then
            ln -sf ../../../config_isap9/$file test/$model/config/$file
        fi
    done
fi
tree test/$model

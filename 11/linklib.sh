#!/bin/bash
model=$1
path=model_isap/kennet/$model
if [ -e $path/fr.db ];then
    ln -sf ../../../../$path/fr.db test/$model/config/models/fr.db
fi
if [ -e $path/libkenxnet.so ];then
    ln -sf ../../../$path/libkenxnet.so test/$model/lib/libkenxnet.so
fi
tree test/$model


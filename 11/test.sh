#!/bin/bash
code=$1
model=$2
dataset=ijba11
if [ "$3" != "" ]; then
    dataset=$3
fi
echo dataset=$dataset
testfold=./test/$model
scripts/build_isap.sh $code $testfold/lib debug
scripts/compile_and_link.sh $testfold debug
./run_custom_test.sh $testfold $model $dataset
./cal_ijball_new.py $model

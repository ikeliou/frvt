#!/bin/bash
array=(./model_isap/kennet/*)
echo ${array[@]}
for a in ${array[@]};
do
    model=`basename $a`
    echo $model
    ./cal_ijball_new.py $model
done

#!/bin/bash
#models=(20191123-002224-step274 20191122-150359-step366 20191111-131800-step_173)
#models=(20191122-150359-step188 20191125-162655 20191126-105334)
models=(20191202-145806-360)
for ((i=0;i<${#models[@]};++i)); do
    model="${models[i]}"
    echo model $model
    tail -n 2 model_isap/ijba11_$model/report_new.txt
done

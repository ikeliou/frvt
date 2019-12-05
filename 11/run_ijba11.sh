#!/bin/bash

#models=(ike_mtcnn_very_first)
#models=(Model_0004)
#models=(Model_0003_retinaface)
#models=(Model_0000_retinaface)
#models=(Model_0001_retinaface Model_0002_retinaface Model_0003_retinaface Model_0004_retinaface)
#models=(Model_0005 Model_0006)
#models=(Model_0005_retinaface_mobile)
#models=(Model_0003_retinaface_mobile)
#models=(20191109-000924 20191109-000953)
#codes=(6 6)
#models=(20191109-000953)
#codes=(6)
models=(20191111-131800-step_173)
#models=(20191116-203546-step162)
codes=(6)
#models=(20191111-131800-step_173_retinaface_mobile)
#models=(20191111-131800-step_524)
#codes=(6)
#models=(20191112-102138-step_191)
#codes=(6)
#echo ${#models[@]}
#for ((i=0;i<${#models[@]};++i)); do
#    echo $i
#done
#exit
for ((i=0;i<${#models[@]};++i)); do
#for model in "${models[@]}"; do
    model="${models[i]}"
    code="${codes[i]}"
    ln -sfT lib_isap$code lib
    ln -sfT config_isap$code config
    libso=model_isap/kennet/"$model"/libkenxnet.so
    echo $libso
	if [ -e $libso ]; then
		ln -sf ../model_isap/kennet/"$model"/libkenxnet.so ./lib/libkenxnet.so 
	else
		ln -sf ../model_isap/kennet/Model_0000/libkenxnet.so ./lib/libkenxnet.so
	fi
	frdb=model_isap/kennet/"$model"/fr.db
	echo $frdb
	if [ -e $frdb ]; then
		ln -sf ../../$frdb ./config/models/fr.db
	fi
    ./scripts/build_isap_impl"$code".sh
    ./scripts/compile_and_link.sh
#: << 'end'
	for i in {1..10};
	do
		folder=./model_isap/ijba11_"$model"/ijba11_split"$i"_"$model"_validation
		echo $folder
		mkdir -p "$folder"
		./run_enroll_test.sh input/ijba11_split"$i"_enroll.txt "$folder"
		./run_match_test.sh input/ijba11_split"$i"_match.txt "$folder"
	done
#end
done

#!/bin/bash

testid=6
ln -sfT ./config_isap$testid ./config
ln -sfT ./lib_isap$testid ./lib
scripts/build_isap_impl"$testid".sh
scripts/compile_and_link.sh
ln -sf ../model_isap/kennet/Model_0000/libkenxnet.so ./lib/libkenxnet.so
#models=(Model_0000 Model_0001 Model_0002 Model_0003)
models=(Model_0003_retinaface)
#models=(Model_0003)
for model in "${models[@]}"; do
	frdb=model_isap/kennet/"$model"/fr.db
	echo $frdb
	if [ -e $frdb ]; then
		ln -sf ../../$frdb ./config/models/fr.db
	fi
	#ln -sf ../../model_isap/kennet/"$model"/fr.db ./config/models/fr.db
	folder=./model_isap/"$model"/ijbc_test1_"$model"/ijbc_test1_split1_"$model"_validation
	echo $folder
	mkdir -p "$folder"
	./run_enroll_test.sh input/ijbc_test1_enroll.txt "$folder"
	mv $folder/enroll.log.0 $folder/enroll.log
	./run_enroll_test.sh input/ijbc_test1_verif.txt "$folder"
	mv $folder/enroll.log.0 $folder/verif.log
	./run_match_test.sh input/ijbc_test1_match.txt "$folder"
done

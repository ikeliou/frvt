#!/bin/bash

#isap
cp ./model_isap/lib/libfrvt_11_isap_001.so ./lib
cp ./model_isap/kennet/libkenxnet.so ./lib
#cp ./model_isap/lib/libkenxnet.so ./lib
mkdir -p ./config/models
cp ./model_isap/kennet/fr.db ./config/models
#cp ./model_isap/config/models/fr.db ./config/models
cp ./model_isap/config/models/fc.db ./config/models
./scripts/compile_and_link.sh

mkdir -p my_ijba11_validation
./run_enroll_test.sh input/my_ijba11_enroll.txt my_ijba11_validation
mkdir -p my_ijba11_validation_1
./run_enroll_test.sh input/my_ijba11_enroll.txt my_ijba11_validation_1
#./run_enroll_test.sh input/my_enroll_small.txt my_validation
./run_match_test.sh input/my_ijba11_match.txt my_ijba11_validation

#mkdir -p my_eyes
#python ./mark_eyes.py my_validation/enroll.log.0 my_eyes

#!/bin/bash
code=$1
#if [ code -ge 5 ]; then
#	ln -sf ../model_isap/kennet/Model_0000/libkenxnet.so ./lib/libkenxnet.so
#	ln -sf ../../model_isap/kennet/Model_0000/fr.db ./config/models/fr.db
#fi
testfold=test/isap"$code"
#ln -sfT ../../config_isap$code $testfold/config
#ln -sfT ../../lib_isap$code $testfold/lib
#scripts/build_isap_impl"$code".sh
scripts/build_isap.sh $code $testfold/lib debug
scripts/compile_and_link.sh $testfold debug
tests=(my_ijba11)
#tests=(my)
#tests=(my_test)
for name in "${tests[@]}"; do
	mkdir -p "$name"_validation
	#./run_enroll_test.sh input/"$name"_enroll.txt "$name"_validation
    inputFile=input/"$name"_enroll.txt
    outputDir="$name"_validation
    tempConfigDir=$testfold/config
    outputStem=enroll
    numForks=1
    templatesDir=$outputDir/templates
    mkdir -p $templatesDir
    $testfold/bin/validate11 createTemplate -x enroll -c $tempConfigDir -o $outputDir -h $outputStem -i $inputFile -t $numForks -j $templatesDir
	mkdir -p "$name"_eyes
	python ./mark_eyes.py "$name"_validation/enroll.log.0 "$name"_eyes
	#./run_match_test.sh input/"$name"_match.txt "$name"_validation
    $testfold/bin/validate11 match -c $tempConfigDir -o $outputDir -h match -i input/"$name"_match.txt -t $numForks -j $templatesDir
done


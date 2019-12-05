#!/bin/bash
testfold=test/20191111-131800-step_173_retina_with_posechoose
outputDir=mafa_validation
outputStem=enroll
inputFile=input/mafa_enroll.txt
numForks=1

if [ $# == 2 ]; then
	#inputFile=$1
	#outputDir=$2
    testfold=$1
    outputDir=$2
fi
#echo $inputFile $outputDir
echo $testfold $outputDir
templatesDir=$outputDir/templates
tempConfigDir=$testfold/config
mkdir -p $templatesDir
#./scripts/compile_and_link.sh
$testfold/bin/validate11 createTemplate -x enroll -c $tempConfigDir -o $outputDir -h $outputStem -i $inputFile -t $numForks -j $templatesDir


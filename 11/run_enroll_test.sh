#!/bin/bash
tempConfigDir=config
outputDir=my_validation
outputStem=enroll
inputFile=input/my_enroll.txt
numForks=1

if [ $# == 2 ]; then
	inputFile=$1
	outputDir=$2
fi
echo $inputFile $outputDir
templatesDir=$outputDir/templates
mkdir -p $templatesDir
./scripts/compile_and_link.sh
bin/validate11 createTemplate -x enroll -c $tempConfigDir -o $outputDir -h $outputStem -i $inputFile -t $numForks -j $templatesDir


#!/bin/bash
configDir=config
outputDir=./sd32_validation
outputStem=match
inputFile=./input/NIST_SD32_MEDS-II_face_match.txt
numForks=1

if [ $# == "2" ]; then
	inputFile=$1
	outputDir=$2
fi
templatesDir=$outputDir/templates

echo $inputFile $outputDir
./scripts/compile_and_link.sh
bin/validate11 match -c $configDir -o $outputDir -h $outputStem -i $inputFile -t $numForks -j $templatesDir

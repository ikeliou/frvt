#!/bin/bash
testfold=$1
model=$2
dataset=$3
split_num=10
if [ "$dataset" == "ijba11" ]; then
    split_num=10
elif [ "$dataset" == "ijbc_test1" ]; then
    split_num=1 
else
    echo dataset not exist
    exit
fi
echo dataset=$dataset
echo split_num=$split_num
#for i in {1..10};
for i in $(seq 1 $split_num);
do
    echo $i
    outputDir=./model_isap/"$dataset"_"$model"/"$dataset"_split"$i"_"$model"_validation
    echo $outputDir
    mkdir -p "$outputDir"
    configDir=$testfold/config
    numForks=1
    templatesDir=$outputDir/templates
    mkdir -p $templatesDir

    inputFile=./input/"$dataset"_split"$i"_enroll.txt
    outputStem=enroll
    $testfold/bin/validate11 createTemplate -x enroll -c $configDir -o $outputDir -h $outputStem -i $inputFile -t $numForks -j $templatesDir

    inputFile=input/"$dataset"_split"$i"_match.txt
    outputStem=match
    $testfold/bin/validate11 match -c $configDir -o $outputDir -h $outputStem -i $inputFile -t $numForks -j $templatesDir
done


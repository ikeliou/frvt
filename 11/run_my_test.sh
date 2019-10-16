#!/bin/bash
#./run_enroll_test.sh input/my_enroll.txt my_validation
./run_enroll_test.sh input/my_enroll_small.txt my_validation

rm ./eyes_test/*.jpg
python ./mark_eyes.py my_validation/enroll.log.0 eyes_test

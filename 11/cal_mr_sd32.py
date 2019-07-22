#!/usr/bin/python
from tool import calculator
validation_dir="./sd32_validation"
input_dir="./input"
matchlog = validation_dir+"/match_small.log.0"
enrollinput = input_dir+"/NIST_SD32_MEDS-II_face_enroll_small.txt"
cal = calculator(matchlog, enrollinput)


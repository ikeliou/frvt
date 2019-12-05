#!/usr/bin/python
import sys
import cv2
import os
import fnmatch
import pandas as pd
from tool import *
IJBC_PATH = "../../IJB/IJB-C"
IJBC_ENROLL_FILE = "../../IJB/IJB-C/protocols/test1/enroll_templates.csv"
IJBC_VERIF_FILE = "../../IJB/IJB-C/protocols/test1/verif_templates.csv"
IJBC_MATCH_FILE = "../../IJB/IJB-C/protocols/test1/match.csv"
MY_IJBC_TEST_PATH = "../../ijbc/test1"
#generate_enroll_img_ijbc11_test(IJBC_PATH, IJBC_ENROLL_FILE, MY_IJBC_TEST_PATH+"/enroll")
#generate_enroll_img_ijbc11_test(IJBC_PATH, IJBC_VERIF_FILE, MY_IJBC_TEST_PATH+"/verif")
generate_enroll_data_ijbc11_test(MY_IJBC_TEST_PATH+"/enroll", "./input/ijbc_test1_enroll.txt")
#generate_enroll_data_ijbc11_test(MY_IJBC_TEST_PATH+"/verif", "./input/ijbc_test1_verif.txt")
#generate_match_data_ijbc11_test(IJBC_MATCH_FILE, "./input/ijbc_test1_match.txt")

#!/usr/bin/python
import sys
import cv2
import os
import fnmatch
import pandas as pd
from tool import *

IJBC_PATH = "../../IJB/IJB-C"
IJBC_ENROLL_FILE = "../../IJB/IJB-C/protocols/test2/enroll_templates.csv"
MY_IJBC_TEST_PATH = "../../ijbc/test2"
generate_enroll_img_ijbc11_test(IJBC_PATH, IJBC_ENROLL_FILE, MY_IJBC_TEST_PATH)

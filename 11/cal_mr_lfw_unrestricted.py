#!/usr/bin/python
import pandas as pd
import math
import numpy
import bisect
import sys
import numpy as np
from sklearn.metrics import roc_auc_score
from plot_roc_curve import plot_roc_curve
from sklearn.metrics import roc_curve, auc
from tool import *

validation_dir="./lfw_validation"
input_dir="./input"
if len(sys.argv)==3:
	validation_dir=sys.argv[1]
	input_dir=sys.argv[2]
matchlog = validation_dir+"/match.log.0"
enrollinput = input_dir+"/lfw_enroll.txt"
def lfw_handle(img):
	return img.split("/")[4]

y, scores = get_lables_and_scores(enrollinput, matchlog, lambda img: img.split("/")[4])
line = open("/home/data/people.txt").readline()
round=int(line.split()[0])
count=int(line.split()[1])
print(scores)
print(line)

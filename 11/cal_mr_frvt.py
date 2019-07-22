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
enrollinput = "./input/enroll.txt"
verifinput = "./input/verif.txt"
matchlog = "./validation/match.log"
if len(sys.argv)==3:
	enrollinput=sys.argv[1]
	matchlog=sys.argv[2]

y, scores = get_lables_and_scores([enrollinput, verifinput], matchlog, lambda img: img[img.find("S"):img.find("-")])
fpr, tpr, _ = roc_curve(y, scores)
roc_auc = auc(fpr, tpr)
for a,b in zip(fpr,tpr):
	if a!=0 and b!=0:
		print("nfmr="+str(1.-b)+" when fmr="+str(a))
		print(a,b)
		break;
print("auc:", roc_auc)
plot_roc_curve(y, scores, "./frvt_roc.png")

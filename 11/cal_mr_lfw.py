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
enrollinput = "./input/lfw_enroll.txt"
matchlog = "./lfw_validation/match.log.0"
if len(sys.argv)==3:
	enrollinput=sys.argv[1]
	matchlog=sys.argv[2]

def lfw_handle(img):
	return img.split("/")[4]

y, scores = get_lables_and_scores(enrollinput, matchlog, lambda img: img.split("/")[4])
line = open("/home/data/pairs.txt").readline()
round=int(line.split()[0])
count=int(line.split()[1])
#print(scores)
for threshold in np.arange(0.1,1.,0.05):
	acc=[]
	print("threshold",threshold)
	for i in range(round):
		j=i*count*2
		#print(j,j+count)
		#print(j+count,j+2*count)
		data1=scores[j:j+count]
		data2=scores[j+count:j+2*count]
		#print("round ",i)
		#print(sum(data1)/count)
		#print(sum(data2)/count)
		count1=[1 if score>=threshold else 0 for score in data1]
		count2=[1 if score<threshold else 0 for score in data2]
		true_count=sum(count1)+sum(count2)
		acc.append(float(true_count)/float(2*count))
	print("acc",np.mean(acc))
	print("std",np.std(acc))

fpr, tpr, _ = roc_curve(y, scores)
roc_auc = auc(fpr, tpr)
#print(fpr)
#print(tpr)
for a,b in zip(fpr,tpr):
	if a!=0 and b!=0:
		print("nfmr="+str(1.-b)+" when fmr="+str(a))
		print(a,b)
		break;
print("auc:", roc_auc)
plot_roc_curve(y, scores, "./lfw_roc.png")

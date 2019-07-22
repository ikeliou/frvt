#!/usr/bin/python
import pandas as pd
import math
import numpy
import bisect
import sys
import numpy as np
from sklearn.metrics import roc_auc_score
from plot_roc_curve import plot_roc_curve
validation_dir="./validation"
input_dir="./input"
if len(sys.argv)==3:
	validation_dir=sys.argv[1]
	input_dir=sys.argv[2]

matchlog = validation_dir+"/match.log"
enrollinput = input_dir+"/enroll.txt"
verifinput = input_dir+"/verif.txt"

match_df = pd.read_csv(matchlog, sep = " ")
enroll_df = pd.read_csv(enrollinput, header=None, sep = " ")
verif_df = pd.read_csv(verifinput, header=None, sep = " ")


enroll_df.columns = ["id","image","type"]
enroll_images = [img[img.find("S"):img.find("-")] for img in enroll_df["image"].tolist()]
enroll_ids = enroll_df["id"][:662].tolist()
enroll_dict = dict(zip(enroll_ids,enroll_images))
enroll_img_dict = dict(zip(enroll_ids,enroll_df["image"].tolist()))
#print(enroll_df["id"][:662].tolist())
verif_df.columns = ["id","image","type"]
verif_images = [img[img.find("S"):img.find("-")] for img in verif_df["image"].tolist()]
verif_ids = verif_df["id"].tolist()
verif_dict = dict(zip(verif_ids,verif_images))
verif_img_dict = dict(zip(verif_ids,verif_df["image"].tolist()))
#print(verif_dict)
genuine_scores = []
impostor_scores = []

gN = 0
iN = 0
msg1 = ''
msg2 = ''
y_true = []
y_scores = []
for index, row in match_df.iterrows():
	id1=row["enrollTempl"]
	id1=int(id1[:id1.find(".")])
	id2=row["verifTempl"]
	id2=int(id2[:id2.find(".")])
	score=float(row["simScore"])
	y_true.append(1 if enroll_dict[id1]==verif_dict[id2] else 0)
	y_scores.append(score)
	#print(id1,id2,row["returnCode"])
	if enroll_dict[id1]==verif_dict[id2]:
		gN+=1
		if score < 0.4:
			msg1+=str(('genuine', enroll_img_dict[id1], verif_img_dict[id2], score))+'\n'
		if row["returnCode"]==0 and score!=0.:
			genuine_scores.append(score)
	else:
		iN+=1
		if score > 0.6:
			msg2+=str(('imposter',enroll_img_dict[id1], verif_img_dict[id2], score))+'\n'
		if row["returnCode"]==0 and score!=0.:
			impostor_scores.append(score)
	if row["returnCode"]!=0:
		print("wrong match: ",row)
def fmr_range(lower,upper,K):
	ret=[]
	for i in range(1,K+1):
		c=float(i)/float(K)
		v=math.log10(lower)+c*(math.log10(upper)-math.log10(lower))
		print(c,v)
		v=math.pow(10,v)
		ret.append(v)
	return ret
print(msg1)
print(msg2)
#print('genuine_scores',genuine_scores)
#print('impostor_scores',impostor_scores)
#exit()
#print(fmr_range(0.01,0.99,5))

def cal_fmr(scores,N,T,append):
	arr=[1 if score-T>=0 else 0 for score in scores]
	arr+=append
	#print(arr)
	return float(sum(arr))/float(N)

gappend=[0 for _ in range(gN-len(genuine_scores))]
iappend=[1 for _ in range(iN-len(impostor_scores))]

print(gN, len(genuine_scores))
print(iN, len(impostor_scores))
lo=min(min(impostor_scores),min(genuine_scores))
hi=max(max(impostor_scores),max(genuine_scores))
step=(hi-lo)/100
fmr_data=[]
print(lo,hi);
for c in numpy.arange(hi+step,lo-step,-step):
	v=cal_fmr(impostor_scores,iN,c,iappend)
	if v==0:
		continue;
	if v==1:
		break;
	if len(fmr_data)>0 and v==fmr_data[-1][0]:
		#fmr_data[-1]=(v,c)
		continue
	fmr_data+=[(v,c)]
#fmr_data=[(cal_fmr(impostor_scores,iN,c,iappend),c) for c in numpy.arange(hi+2*step,lo-2*step,-step)]
print(fmr_data)
#print(lo,hi)
def cal_nfmr(fmr,fmr_data):
	fmrs=[data[0] for data in fmr_data]
	pair=fmr_data[bisect.bisect_left(fmrs, fmr)]
	c=pair[1]
	print('threshold:',c)
	return 1.-cal_fmr(genuine_scores,gN,c,gappend)

print("fmr range from "+str(fmr_data[0][0])+" to "+str(fmr_data[-1][0]))
print("nfmr="+str(cal_nfmr(fmr_data[0][0],fmr_data))+" when fmr="+str(fmr_data[0][0]))
print("nfmr="+str(cal_nfmr(0.1,fmr_data))+" when fmr=0.1")
print("nfmr="+str(cal_nfmr(0.5,fmr_data))+" when fmr=0.5")
print("nfmr="+str(cal_nfmr(0.9,fmr_data))+" when fmr=0.9")

print(roc_auc_score(y_true, y_scores))
plot_roc_curve(y_true, y_scores, './roc.png')
#exit()
#print("nfmr="+str(cal_nfmr(fmr_data[100][0],fmr_data))+" when fmr="+str(fmr_data[100][0]))
#print("nfmr="+str(cal_nfmr(fmr_data[200][0],fmr_data))+" when fmr="+str(fmr_data[200][0]))
'''
t=0.5633277656
nfmr=1.-cal_fmr(genuine_scores,gN,t,gappend)
fmr=cal_fmr(impostor_scores,iN,t,iappend)
print("nfmr:",nfmr)
print("fmr:",fmr)
'''

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
import glob

def ijba11_handle(verify_metadata):
	#fname=img.split("/")[-1]
	#sid=fname.split("_")[0]
	df=pd.read_csv(verify_metadata, sep = ",")
	enroll_dict={}
	enroll_img_dict={}
	for index, row in df.iterrows():
		tid=row["TEMPLATE_ID"]
		sid=row["SUBJECT_ID"]
		img=row["FILE"]
		a,b=img.split("/")
		if a=="frame":
			img="frames/"+b
		enroll_dict[tid]=sid
		enroll_img_dict[tid]=img
	return enroll_dict, enroll_img_dict

data={
    "helper": ijba11_handle,
    "num": 10,
    "verify_metadata":"../../IJB/IJB-A/IJB-A_11_sets/split%d/verify_metadata_%d.csv",}

data["models"]=[
	{"path":"./model_isap/ijba11_"+sys.argv[1],"validation":"ijba11_split%d_"+sys.argv[1]+"_validation"},]

helper=data["helper"]
num=data["num"]
models=data["models"]
for model in models:
    validation=os.path.join(model["path"],model["validation"])
    report_buf=""
    fnmr_dict={"0.01":[]}
    for i in range(1,num+1):
        print(data["verify_metadata"])
        verify_metadata=data["verify_metadata"]
        if num>1:
            verify_metadata=verify_metadata%(i,i)
        matchlog=os.path.join(validation%(i),"match.log.0")
        match_df = pd.read_csv(matchlog, sep = " ")
        print("verify_metadata",verify_metadata)
        print("matchlog",matchlog)
        enroll_dict, enroll_img_dict = helper(verify_metadata)
        y, scores = get_lables_and_scores_from_df(enroll_dict, enroll_img_dict, match_df)
        print("len",len(scores))
        fpr, tpr, _ = roc_curve(y, scores)
        roc_auc = auc(fpr, tpr)
        report_buf+="split%d\n"%(i)
        for a,b in zip(fpr,tpr):
            if a>=0.01:
                fnmr=1.-b
                fmr=a
                report_buf+="fnmr="+str(fnmr)+" when fmr="+str(fmr)+"\n"
                #print(a,b)
                fnmr_dict["0.01"].append(fnmr)
                break;
        report_buf+="auc=%f\n"%(roc_auc)
        #plot_roc_curve(y, scores, "./ijba11_roc.png")
    for fmr,fnmr_arr in fnmr_dict.items():
        mean=np.mean(fnmr_arr)
        std=np.std(fnmr_arr)
        report_buf+="average fnmr: %f std: %f, when fmr=%s\n"%(mean,std,fmr)
    fd=open(os.path.join(model["path"],"report.txt"), "w+")
    fd.write(report_buf)
    print(report_buf)
exit()

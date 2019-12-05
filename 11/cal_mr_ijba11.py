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

def ijbc_handle(verify_metadata):
	#fname=img.split("/")[-1]
	#sid=fname.split("_")[0]
	enroll_dict={}
	enroll_img_dict={}
	for metadata in verify_metadata:
		df=pd.read_csv(metadata, sep = ",")
		for index, row in df.iterrows():
			tid=row["TEMPLATE_ID"]
			sid=row["SUBJECT_ID"]
			img=row["FILENAME"]
			a,b=img.split("/")
			if a=="frame":
				img="frames/"+b
			enroll_dict[tid]=sid
			enroll_img_dict[tid]=img
	return enroll_dict, enroll_img_dict

#ijba11_validations=["./ijba11_*_validation"]
datasets={
	"ijba11":{
		"helper": ijba11_handle,
		"num": 10,
		"verify_metadata":"../../IJB/IJB-A/IJB-A_11_sets/split%d/verify_metadata_%d.csv",
		#"verify_metadata":"../../IJB/IJB-A/IJB-A_11_sets/split1/verify_metadata_1.csv",
		"models":[
			#{"path":"./model_isap/ijba11_first","validation":"ijba11_split%d_first_validation"},
			#{"path":"./model_isap/ijba11_ike_mtcnn_very_first","validation":"ijba11_split%d_ike_mtcnn_very_first_validation"},
			#{"path":"./model_isap/ijba11_LFW_99.650","validation":"ijba11_split%d_1_validation"},
			#{"path":"./model_isap/ijba11_Model_0000","validation":"ijba11_split%d_validation"},
			#{"path":"./model_isap/ijba11_Model_0001","validation":"ijba11_split%d_Model_0001_validation"},
			#{"path":"./model_isap/ijba11_Model_0002","validation":"ijba11_split%d_Model_0002_validation"},
			#{"path":"./model_isap/ijba11_Model_0003","validation":"ijba11_split%d_Model_0003_validation"},
			#{"path":"./model_isap/ijba11_Model_0004","validation":"ijba11_split%d_Model_0004_validation"},
			#{"path":"./model_isap/ijba11_Model_0000_retinaface","validation":"ijba11_split%d_Model_0000_retinaface_validation"},
			#{"path":"./model_isap/ijba11_Model_0000_retinaface","validation":"ijba11_split%d_Model_0000_retinaface_validation"},
			#{"path":"./model_isap/ijba11_Model_0001_retinaface","validation":"ijba11_split%d_Model_0001_retinaface_validation"},
			#{"path":"./model_isap/ijba11_Model_0002_retinaface","validation":"ijba11_split%d_Model_0002_retinaface_validation"},
			#{"path":"./model_isap/ijba11_Model_0003_retinaface","validation":"ijba11_split%d_Model_0003_retinaface_validation"},
			#{"path":"./model_isap/ijba11_Model_0004_retinaface","validation":"ijba11_split%d_Model_0004_retinaface_validation"},
			#{"path":"./model_isap/ijba11_Model_0005","validation":"ijba11_split%d_Model_0005_validation"},
			#{"path":"./model_isap/ijba11_Model_0006","validation":"ijba11_split%d_Model_0006_validation"},
			#{"path":"./model_isap/ijba11_Model_0005_retinaface_mobile","validation":"ijba11_split%d_Model_0005_retinaface_mobile_validation"},
			#{"path":"./model_isap/ijba11_Model_0003_retinaface_mobile","validation":"ijba11_split%d_Model_0003_retinaface_mobile_validation"},
			#{"path":"./model_isap/ijba11_20191109-000924","validation":"ijba11_split%d_20191109-000924_validation"},
			#{"path":"./model_isap/ijba11_20191109-000953","validation":"ijba11_split%d_20191109-000953_validation"},
			#{"path":"./model_isap/ijba11_20191111-131800-step_173","validation":"ijba11_split%d_20191111-131800-step_173_validation"},
			#{"path":"./model_isap/ijba11_20191111-131800-step_173_retinaface_mobile","validation":"ijba11_split%d_20191111-131800-step_173_retinaface_mobile_validation"},
			#{"path":"./model_isap/ijba11_20191112-102138-step_191","validation":"ijba11_split%d_20191112-102138-step_191_validation"},
			{"path":"./model_isap/ijba11_20191111-131800-step_524","validation":"ijba11_split%d_20191111-131800-step_524_validation"},
		],
	},
	"ijbc_test1":{
		"helper": ijbc_handle,
		"num": 1,
		"verify_metadata":["../../IJB/IJB-C/protocols/test1/enroll_templates.csv","../../IJB/IJB-C/protocols/test1/verif_templates.csv"],
		"models":[
			#{"path":"./model_isap/Model_0003/ijbc11_test1_Model_0003","validation":"ijbc11_test1_split%d_Model_0003_validation"},
		],
	}
}

for dname, data in datasets.items():
	print(dname,data)
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


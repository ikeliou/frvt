#!/usr/bin/python
import pandas as pd
import math
import numpy
import bisect
import sys
import fnmatch
import os
import numbers
import cv2
from progressbar import *
class calculator():
	def __init__(self, matchlog, enrollinput):
		match_df = pd.read_csv(matchlog, sep = " ")
		enroll_df = pd.read_csv(enrollinput, header=None, sep = " ")
		enroll_df.columns = ["id","image","type"]
		enroll_images = [img[img.find("S"):img.find("-")] for img in enroll_df["image"].tolist()]
		enroll_ids = enroll_df["id"].tolist()
		enroll_dict = dict(zip(enroll_ids,enroll_images))
		enroll_img_dict = dict(zip(enroll_ids,enroll_df["image"].tolist()))
		genuine_scores = []
		impostor_scores = []

		gN = 0
		iN = 0
		msg1 = ''
		msg2 = ''
		for index, row in match_df.iterrows():
			id1=row["enrollTempl"]
			id1=int(id1[:id1.find(".")])
			id2=row["verifTempl"]
			id2=int(id2[:id2.find(".")])
			score=float(row["simScore"])
			#print(id1,id2,row["returnCode"])
			if enroll_dict[id1]==enroll_dict[id2]:
				gN+=1
				if score < 0.4:
					msg1+=str(('genuine', enroll_img_dict[id1], enroll_img_dict[id2], score))+'\n'
				if row["returnCode"]==0 and score!=0.:
					genuine_scores.append(score)
			else:
				iN+=1
				if score > 0.6:
					msg2+=str(('imposter',enroll_img_dict[id1], enroll_img_dict[id2], score))+'\n'
				if row["returnCode"]==0 and score!=0.:
					impostor_scores.append(score)
			if row["returnCode"]!=0:
				print("wrong match: ",row)
		print(msg1)
		print(msg2)
def generate_enroll_data(imgdir, outpath):
	images = []
	for root, dirnames, filenames in os.walk(imgdir):
		for filename in filenames:
			if filename.endswith(('.jpg', '.jpeg', '.gif', '.png')):
				images.append(os.path.join(root, filename))
	fd=open(outpath, 'w')
	id=1
	img2id = {}
	for image in images:
		fname = os.path.basename(image)
		path = os.path.dirname(image)
		fname_arr = os.path.splitext(fname)
		#print(path,fname,fname_arr[0],fname_arr[1])
		imgpath = path+"/"+fname_arr[0]+".ppm"
		print(imgpath)
		img2id[imgpath] = id
		fd.write(str(id)+" "+imgpath+" MUGSHOT\n")
		id += 1
	print("write to "+outpath)
	return img2id

def get_lables_and_scores_from_df(enroll_dict,enroll_img_dict,match_df):
	genuine_scores = []
	impostor_scores = []

	gN = 0
	iN = 0
	msg1 = ''
	msg2 = ''
	y_true = []
	y_scores = []
	pbar = ProgressBar().start()
	total = match_df.shape[0]
	#print(enroll_dict)
	for index, row in match_df.iterrows():
		id1=row["enrollTempl"]
		id1=int(id1[:id1.find(".")])
		id2=row["verifTempl"]
		id2=int(id2[:id2.find(".")])
		score=float(row["simScore"])
		y_true.append(1 if enroll_dict[id1]==enroll_dict[id2] else 0)
		#print(score,type(score))
		if math.isnan(score):
			score=0
		y_scores.append(score)
		if enroll_dict[id1]==enroll_dict[id2]:
			gN+=1
			if score < 0.4:
				msg1+=str(('genuine', enroll_img_dict[id1], enroll_img_dict[id2], score))+'\n'
				msg1+=str(row)+'\n'
			if row["returnCode"]==0 and score!=0.:
				genuine_scores.append(score)
		else:
			iN+=1
			if score > 0.6:
				msg2+=str(('imposter',enroll_img_dict[id1], enroll_img_dict[id2], score))+'\n'
				msg2+=str(row)+'\n'
			if row["returnCode"]==0 and score!=0.:
				impostor_scores.append(score)
		if row["returnCode"]!=0:
			print("wrong match: ",row)
			exit()
		#print((index / (total - 1.)) * 100.)
		pbar.update((index / (total - 1.)) * 100)

	pbar.finish()
	#print(msg1)
	#print(msg2)
	return y_true, y_scores

def get_lables_and_scores(enrollinput,matchlog,handle_func=None):
	enroll_df = None
	if type(enrollinput)==list:
		enroll_df = pd.read_csv(enrollinput[0], header=None, sep = " ")
		for f in enrollinput[1:]:
			df=pd.read_csv(f, header=None, sep = " ")
			enroll_df=pd.concat([enroll_df,df],axis=0, ignore_index=True)
	else:
		enroll_df = pd.read_csv(enrollinput, header=None, sep = " ")
	match_df = pd.read_csv(matchlog, sep = " ")

	enroll_df.columns = ["id","image","type"]
	enroll_images = [handle_func(img) for img in enroll_df["image"].tolist()]
	enroll_ids = enroll_df["id"].tolist()
	enroll_dict = dict(zip(enroll_ids,enroll_images))
	enroll_img_dict = dict(zip(enroll_ids,enroll_df["image"].tolist()))

	genuine_scores = []
	impostor_scores = []

	gN = 0
	iN = 0
	msg1 = ''
	msg2 = ''
	y_true = []
	y_scores = []
	pbar = ProgressBar().start()
	total = match_df.shape[0]
	#print(enroll_dict)
	for index, row in match_df.iterrows():
		id1=row["enrollTempl"]
		id1=int(id1[:id1.find(".")])
		id2=row["verifTempl"]
		id2=int(id2[:id2.find(".")])
		score=float(row["simScore"])
		y_true.append(1 if enroll_dict[id1]==enroll_dict[id2] else 0)
		y_scores.append(score)
		if enroll_dict[id1]==enroll_dict[id2]:
			gN+=1
			if score < 0.4:
				msg1+=str(('genuine', enroll_img_dict[id1], enroll_img_dict[id2], score))+'\n'
				msg1+=str(row)+'\n'
			if row["returnCode"]==0 and score!=0.:
				genuine_scores.append(score)
		else:
			iN+=1
			if score > 0.6:
				msg2+=str(('imposter',enroll_img_dict[id1], enroll_img_dict[id2], score))+'\n'
				msg2+=str(row)+'\n'
			if row["returnCode"]==0 and score!=0.:
				impostor_scores.append(score)
		if row["returnCode"]!=0:
			print("wrong match: ",row)
			exit()
		#print((index / (total - 1.)) * 100.)
		pbar.update((index / (total - 1.)) * 100)

	pbar.finish()
	#print(msg1)
	#print(msg2)
	return y_true, y_scores

def generate_enroll_img_ijbc11_test(ijbc_path,ijbc_enroll_file,out_path):
	df = pd.read_csv(ijbc_enroll_file, sep = ",")
	for index,row in df.iterrows():
		#print(row)
		tid=row["TEMPLATE_ID"]
		sid=row["SUBJECT_ID"]
		x=int(row["FACE_X"])
		y=int(row["FACE_Y"])
		w=int(row["FACE_WIDTH"])
		h=int(row["FACE_HEIGHT"])
		fname=row["FILENAME"]
		clip_path=os.path.join(out_path,str(tid))
		if not os.path.exists(clip_path):
			os.makedirs(clip_path)
		img_file=os.path.join(ijbc_path,"images",fname)
		print(img_file)
		img = cv2.imread(img_file)
		crop_img = img[y:y+h, x:x+w]
		f=fname.split("/")[1].split(".")[0]
		cv2.imwrite(os.path.join(clip_path,str(sid)+"_"+f+'.ppm'), crop_img)
def generate_enroll_data_ijbc11_test(my_ijbc_test_path,input_file):
	buf=""
	for root, dirnames, filenames in os.walk(my_ijbc_test_path):
		if root==my_ijbc_test_path:
			continue
		tid=root.split("/")[-1]
		print(tid)
		print(root, dirnames, filenames)
		buf+=tid
		for f in filenames:
			buf+=" "+root+"/"+f+" MUGSHOT"
		buf+="\n"
	print(buf)
	fd = open(input_file,"w+")
	fd.write(buf)
def generate_match_data_ijbc11_test(ijbc_match_file,input_file):
	df = pd.read_csv(ijbc_match_file, sep = ",")
	buf=""
	for index,row in df.iterrows():
		print(row[0],row[1])
		buf+=str(row[0])+".template"+" "+str(row[1])+".template\n"
	print(buf)
	fd = open(input_file,"w+")
	fd.write(buf)

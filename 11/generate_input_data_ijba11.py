#!/usr/bin/python
import sys
import cv2
import os
import fnmatch
import pandas as pd

def generate_enroll_img_ijba11(ijba_path,out_path):
	for i in range(1,11):
		path=os.path.join(ijba_path,"IJB-A_11_sets","split"+str(i),"verify_metadata_"+str(i)+".csv")
		print(path)
		df = pd.read_csv(path, sep = ",")
		for index,row in df.iterrows():
			#print(row)
			tid=row["TEMPLATE_ID"]
			sid=row["SUBJECT_ID"]
			x=int(row["FACE_X"])
			y=int(row["FACE_Y"])
			w=int(row["FACE_WIDTH"])
			h=int(row["FACE_HEIGHT"])
			fname=row["FILE"]
			prefix, ext= os.path.splitext(fname)
			d, f=os.path.split(prefix)
			if d=="frame":
				fname=os.path.join("frames",f+ext)
			clip_path=os.path.join(out_path,"split"+str(i),str(tid))
			print(d, f, ext)
			print(clip_path)
			if not os.path.exists(clip_path):
				os.makedirs(clip_path)
			img_file=os.path.join(ijba_path,"images",fname)
			print(img_file)
			img = cv2.imread(img_file)
			crop_img = img[y:y+h, x:x+w]
			cv2.imwrite(os.path.join(clip_path,str(sid)+"_"+f+'.ppm'), crop_img)

def generate_enroll_data_ijba11(my_ijba11_path,input_path):
	for i in range(1,11):
		path=os.path.join(my_ijba11_path,"split"+str(i))
		buf=""
		for root, dirnames, filenames in os.walk(path):
			tid=root.split("/")[-1]
			if tid[:5]=="split":
				continue
			print(tid)
			print(root, dirnames, filenames)
			buf+=tid
			for f in filenames:
				buf+=" "+root+"/"+f+" MUGSHOT"
			buf+="\n"
		print(buf)
		fd = open(os.path.join(input_path,"ijba11_split"+str(i)+"_enroll.txt"),"w+")
		fd.write(buf)
def generate_match_data_ijba11(ijba11_path,input_path):
	for i in range(1,11):
		path=os.path.join(ijba11_path,"IJB-A_11_sets","split"+str(i),"verify_comparisons_"+str(i)+".csv")
		print(path)
		df = pd.read_csv(path, sep = ",")
		buf=""
		for index,row in df.iterrows():
			print(row[0],row[1])
			buf+=str(row[0])+".template"+" "+str(row[1])+".template\n"
		print(buf)
		fd = open(os.path.join(input_path,"ijba11_split"+str(i)+"_match.txt"),"w+")
		fd.write(buf)

IJBA_PATH = "../../IJB/IJB-A"
MY_IJBA11_PATH = "../../ijba11"
#generate_enroll_img_ijba11(IJBA_PATH,MY_IJBA11_PATH)
generate_enroll_data_ijba11(MY_IJBA11_PATH,"./input")
generate_match_data_ijba11(IJBA_PATH,"./input")

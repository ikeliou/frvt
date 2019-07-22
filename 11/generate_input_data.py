#!/usr/bin/python
import sys
import cv2
import os
import fnmatch
#from pathlib import Path

#imgdir = "/home/data/sd18"
imgdir = "/home/data/NIST_SD32_MEDS-II_face"
outpath = "./input"
dataname = os.path.split(imgdir)[1]

enroll_txt = outpath+"/"+dataname+"_enroll.txt";
match_txt = outpath+"/"+dataname+"_match.txt";
print(enroll_txt, match_txt)
if len(sys.argv)==3:
	imgdir=sys.argv[1]
	outpath=sys.argv[2]
images = []
for root, dirnames, filenames in os.walk(imgdir):
	for filename in filenames:
		if filename.endswith(('.jpg', '.jpeg', '.gif', '.png')):
			images.append(os.path.join(root, filename))
print(images)
enroll_file = open(enroll_txt, "w")
id=1
for image in images:
	fname = os.path.basename(image)
	path = os.path.dirname(image)
	fname_arr = os.path.splitext(fname)
	print(path,fname,fname_arr[0],fname_arr[1])
	
	# covert ot frvt ppm
	'''
	img = cv2.imread(image)
	cv2.imwrite(path+"/"+fname_arr[0]+".ppm",img)
	infile = open(path+"/"+fname_arr[0]+".ppm",'r').readlines()
	with open(path+"/"+fname_arr[0]+".ppm",'w') as outfile:
		for index,line in enumerate(infile):
			if index != 1:
				outfile.write(line)
	'''	
	imgpath = path+"/"+fname_arr[0]+".ppm"
	enroll_file.write(str(id)+" "+imgpath+" MUGSHOT\n")
	id += 1

match_file = open(match_txt, "w")
for id1 in range(1,id):
	for id2 in range(1,id):
		if id1==id2:
			continue;
		match_file.write(str(id1)+".template "+str(id2)+".template\n")


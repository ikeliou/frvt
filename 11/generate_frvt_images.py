#!/usr/bin/python
import sys
import cv2
import os
import fnmatch
imgdir = "/home/data/lfw"
images = []
for root, dirnames, filenames in os.walk(imgdir):
	for filename in filenames:
		if filename.endswith(('.jpg', '.jpeg', '.gif', '.png')):
			images.append(os.path.join(root, filename))
for image in images:
	fname = os.path.basename(image)
	path = os.path.dirname(image)
	fname_arr = os.path.splitext(fname)
	print(path,fname,fname_arr[0],fname_arr[1])
	
	# covert ot frvt ppm
	img = cv2.imread(image)
	cv2.imwrite(path+"/"+fname_arr[0]+".ppm",img)
	infile = open(path+"/"+fname_arr[0]+".ppm",'r').readlines()
	with open(path+"/"+fname_arr[0]+".ppm",'w') as outfile:
		for index,line in enumerate(infile):
			if index != 1:
				outfile.write(line)

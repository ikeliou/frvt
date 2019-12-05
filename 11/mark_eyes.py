#!/usr/bin/python
import pandas as pd
import math
import numpy
import bisect
import cv2
import os
import sys

enrolllog = "./validation/enroll.log"
outpath = "./eyes4"
if len(sys.argv)==3:
	enrolllog=sys.argv[1]
	outpath=sys.argv[2]
print(enrolllog, outpath)
enroll_df = pd.read_csv(enrolllog, sep = " ")

print(enroll_df)
for index, row in enroll_df.iterrows():
	image = row["image"]
	rc = row["returnCode"]
	xl = row["xleft"]
	yl = row["yleft"]
	xr = row["xright"]
	yr = row["yright"]
	x = row["x"]
	y = row["y"]
	width = row["width"]
	height = row["height"]
	conf = row["conf"]
	img = cv2.imread(image)
	fname = base=os.path.basename(image)
	f = os.path.splitext(fname)
	cv2.circle(img,(xl, yl), 10, (0, 0, 255), -1)
	cv2.circle(img,(xr, yr), 10, (0, 0, 255), -1)
	cv2.rectangle(img, (x, y), (x+width, y+height), (0, 0, 255), 3);
	if conf != 0:
		cv2.putText(img, str(conf), (x, y), cv2.FONT_HERSHEY_SIMPLEX, 0.5, (0, 255, 255))
	print(outpath+"/"+f[0]+".jpg")
	cv2.imwrite(outpath+"/"+f[0]+".jpg",img)
		

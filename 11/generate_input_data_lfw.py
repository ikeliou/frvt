#!/usr/bin/python
import sys
import cv2
import os
import fnmatch
from tool import *

LFW_PATH = "/home/data/lfw"
img2id = generate_enroll_data("/home/data/lfw","./input/lfw_enroll.txt")
#print(img2id)
#pairs_df = pd.read_csv('/home/data/pairs.txt', sep = "\t")
def match_str(name, serial):
	#/home/data/lfw/Colin_Prescot/Colin_Prescot_0001.ppm
	serial_str = ""+serial
	for _ in range(4-len(serial)):
		serial = "0"+serial
	id = img2id[LFW_PATH+"/"+name+"/"+name+"_"+serial+".ppm"]
	return str(id) + ".template"

# restricted
fd = open('/home/data/pairs.txt', 'r')
out_fd = open("./input/lfw_match.txt", 'w')
line = fd.readline().split()
round = int(line[0])
count = int(line[1])
print(round,count)
for _ in range(round):
	for _ in range(count):
		line = fd.readline().split()
		out_fd.write(match_str(line[0],line[1])+" "+match_str(line[0],line[2])+"\n")
	for _ in range(count):
		line = fd.readline().split()
		out_fd.write(match_str(line[0],line[1])+" "+match_str(line[2],line[3])+"\n")

# unrestricted
'''
fd = open('/home/data/people.txt', 'r')
out_fd = open("./input/lfw_match_unrestricted.txt", 'w')
lines = fd.readlines()
round = int(lines[0])
i = 1
for _ in range(round):
	count = int(lines[i])
	this_lines = lines[i+1:i+1+count]
	for j,line1 in enumerate(this_lines):
		for line2 in this_lines[j+1:]:
			name1, id1 = line1.split()
			name2, id2 = line2.split()
			out_fd.write(match_str(name1, id1)+" "+match_str(name2, id2)+"\n")
	i += 1+count
'''

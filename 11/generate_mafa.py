#!/usr/bin/python
import sys
import cv2
import os
import fnmatch
imgdir = "../../mafa"
images = []
for root, dirnames, filenames in os.walk(imgdir):
    for filename in filenames:
        if filename.endswith(('.jpg', '.jpeg', '.gif', '.png')):
            images.append(os.path.join(root, filename))
#print(images)
id=1
data=""
for image in images:
    fname = os.path.basename(image)
    path = os.path.dirname(image)
    fname_arr = os.path.splitext(fname)
    print(path,fname,fname_arr[0],fname_arr[1])
    file=os.path.join(path,fname_arr[0])+".ppm"
    print(file)
    line = "%d %s MUGSHOT\n" % (id, file)
    img = cv2.imread(image)
    #crop_img = img[y:y+h, x:x+w]
    cv2.imwrite(file,img)
    data+=line
    id+=1

fd=open("input/mafa_enroll.txt", 'w')
fd.write(data)

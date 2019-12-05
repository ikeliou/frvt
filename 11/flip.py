#!/usr/bin/python
import cv2
import sys
img1=sys.argv[1]
img2=sys.argv[2]
originalImage = cv2.imread(img1)
flipHorizontal = cv2.flip(originalImage, 1)
cv2.imwrite(img2, flipHorizontal)

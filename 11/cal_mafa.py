#!/usr/bin/python
import pandas as pd
import math
import numpy
import bisect
import sys
import numpy as np
enroll_log=sys.argv[1]
enroll_df = pd.read_csv(enroll_log, sep = " ")
pcount=0.
ncount=0.
for index, row in enroll_df.iterrows():
    if row["xleft"]==0 and row["yleft"]==0 and row["xright"]==0 and row["yright"]==0:
        ncount+=1
    else:
        pcount+=1
print(ncount,pcount)
print("acc:",pcount/(ncount+pcount))

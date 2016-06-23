#!/usr/bin/env python
# extract a single spill from a PADE data file
# Usage: python extractSpill PADE_DATA.txt.(.bz2) spill# (default=1)
# Created 6/23/2016 B.Hirosky: Initial release

import sys, time, os, getopt
from array import array
from ROOT import *
from TBUtils import *


if len(sys.argv)<2:
    print "No PADE data file given"
    sys.exit()
if len(sys.argv)<3:
    target=1
else:
    target=sys.argv[2]
    
padeDatFile=sys.argv[1]
if not os.path.exists(padeDatFile):
    print "File no found",padeDatFile
    sys.exit()
    
try:
    padeDat=TBOpen(padeDatFile)
except IOError as e:
    print "Failed to open PADE data file"
    sys.exit()


outFileName=padeDatFile.replace(".bz2","").replace(".txt","_"+str(target)+".txt")
outFile=open(outFileName,"w")
print "Processing file:", padeDat
print "Writing spill to",outFileName

LoadLibs("TBLIB","libTB.so")

foundSpill=False
while 1:
    padeline = padeDat.readline()
    if "starting spill" in padeline:  # new spill condition
        if foundSpill: break
        spillNum=int(padeline.split()[4])
        if spillNum==target: foundSpill=True
    if not padeline: break
    if foundSpill: outFile.write(padeline)

print "Finished writing spill",target




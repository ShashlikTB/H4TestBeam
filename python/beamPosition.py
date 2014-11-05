#!/usr/bin/env python
# display integrated hits data for Shashlik calorimeter
# this is just a wrapper for the ROOT C++ code
# will replace w/ a simple GUI

# Created 4/12/2014 B.Hirosky: Initial release

import sys
from ROOT import *
from TBUtils import *

logger=Logger()

if len(sys.argv)<2:
    runDat="latest.root"
else: runDat=sys.argv[1]

print "Processing file:",runDat

LoadLibs("TBLIB","libTB.so")
gSystem.SetIncludePath("-I\"$TBHOME/include\"")


gROOT.ProcessLine(".L rootscript/beamPosition.C+")

beamPosition(runDat,1)

hit_continue('Hit any key to exit')













#!/usr/bin/env python

# display channel maps
# this is just a wrapper for the ROOT C++ code

# Created 5/1/2014 B.Hirosky: Initial release

 
from ROOT import *
from TBUtils import *


LoadLibs("TBLIB","libTB.so")


gROOT.ProcessLine(".L rootscript/testChannelMap.C+")


testChannelMap(TBEvent().H4TB_MAP1,"H4TB_MAP1")  # October 29 2014
testChannelMap(TBEvent().H4TB_MAP2,"H4TB_MAP2")  # October 30 2014
testChannelMap(TBEvent().H4TB_MAP3,"H4TB_MAP3")  # October 31 2014
testChannelMap(TBEvent().H4TB_MAP4,"H4TB_MAP4")  # November 01 2014
testChannelMap(TBEvent().H4TB_MAP5,"H4TB_MAP5")  # May 18 2015
testChannelMap(TBEvent().H4TB_MAP6,"H4TB_MAP6")  # May 21 2015



hit_continue('Hit any key to exit')













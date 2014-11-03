#!/usr/bin/env python
# display integrated hits data for Shashlik calorimeter
# this is just a wrapper for the ROOT C++ code
# will replace w/ a simple GUI

# Created 4/12/2014 B.Hirosky: Initial release

import sys
import os.path
from ROOT import *
from TBUtils import *

logger=Logger()

LoadLibs("TBLIB","libTB.so")
gSystem.SetIncludePath("-I\"$TBHOME/include\"")


if len(sys.argv)<2:
    runDat="latest_reco.root"
    rFile=TFile(runDat,"READ")
    if(rFile.IsZombie()):
	print "ERROR: Cannot open latest_reco.root"
	sys.exit()
    tree=rFile.Get("t1041")
    if(tree.IsZombie()):
	print "ERROR: Cannot find t1041 tree"
	sys.exit()
else: 
    runDat=sys.argv[1]
    if ".root" in runDat and 'reco' in runDat:
	rFile=TFile(runDat,"READ")
	if(rFile.IsZombie()):
	  print "ERROR: Cannot open latest_reco.root"
	  sys.exit()
	tree=rFile.Get("t1041")
	if(tree.IsZombie()):
	  print "ERROR: Cannot find t1041 tree"
	  sys.exit()
    if ".root" in runDat and 'reco' not in runDat:
	print 'ERROR: The input root file must be a reco file'
	sys.exit()
    if ".txt" in runDat:
	try: txtFile=open(runDat, 'r')
	except IOError: 'Could not open text file...'
	tree=TChain('t1041')
	for line in txtFile:
	  Line=line.strip()
	  Path=os.getcwd()
	  FullFile=Line #Path + '/' + Line
	  if '.root' in line and 'reco' in line:
	    if os.path.isfile(FullFile):
	      print 'Adding', FullFile, 'to the chain...'
	      tree.Add(FullFile)
	    else: print 'Root+Reco file specified not found, skipping... File:', line
	  else: print 'Line on txt is not a root file or is not a reco file, skipping... File:', FullFile
	if (tree.IsZombie()):
	  print "ERROR: Cannot find t1041 tree"
	  sys.exit()
	if (tree.GetEntries()==0):
	  print "ERROR: Tree is empty"
	  sys.exit()

print
print 'Tree loaded with nEntries:', tree.GetEntries()
print

selectEvent=-1
if len(sys.argv)>2: selectEvent=int(sys.argv[2])
play=False
if selectEvent==999: play=True;

print "Processing file:",runDat


gROOT.ProcessLine(".L rootscript/calCrystalDepo.C+")

calCrystalDepo(tree)
'''
calCrystalDepo(tree, 99)
calCrystalDepo(tree, -99)

for a in range(-16,17):
	if a==0: continue
	else: calCrystalDepo(tree,a)
'''

hit_continue('Hit any key to exit')













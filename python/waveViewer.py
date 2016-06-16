#!/usr/bin/env python
# very rudimentary code to display wave forms 
# Created 4/13/2014 B.Hirosky: Initial release
# modified a lot 6/16/2016: B. Hirosky and T. Anderson - all python now, add selection flags

import sys, getopt
from TBUtils import *
from ROOT import *

def usage():
    print
    print "Usage: python TBTreeMaker [OPTION] <TBTree_filename>.root"
    print "      -b boardID  : restrict display to a given PADE boardID"
    print "      -c chanNum  : restrict display to a given PADE channel number"
    print "      -m minADC   : set cut on minValue of peak ADC sample of signal (eg 120 =~ ped+20)"
    print "      -M maxADC   : set cut on maxValue of peak ADC sample of signal"
    print "      -s sleep    : sleep time between updates in milliseconds [1000]"
    print
    sys.exit()

runDat="latest.root"
if len(sys.argv)<2:
    runDat="latest.root"
else:
    for string in sys.argv:
        if "root" in string:
            runDat=string
            break
    
try:
    opts, args = getopt.getopt(sys.argv[1:], "b:c:m:M:s:H")
except getopt.GetoptError as err: usage()

boardID=-1
chanNum=-1
minADC=-1
maxADC=0xFFFF
sleep=1000 # sleep time between displays in ms
for o, a in opts:
    if o == "-b": boardID=int(a)
    if o == "-c": chanNum=int(a)
    if o == "-m": minADC=int(a)
    if o == "-M": maxADC=int(a)
    if o == "-s": sleep=int(a)
    if o == "-H": usage()
    
    
print "Processing file:",runDat
LoadLibs("TBLIB","libTB.so")
gSystem.SetIncludePath("-I\"$TBHOME/include\"")

gROOT.ProcessLine(".L rootscript/waveViewer.C+")

print "To kill use: control-\ or control-C, kill",os.getpid(),"; fg"

event = TBEvent()
f=TFile(runDat)
t1041=f.Get("t1041")
bevent = t1041.GetBranch("tbevent")
bevent.SetAddress(AddressOf(event))

c=TCanvas("waveViewer","waveViewer")
hw=TH1F("hw","waveform",PadeChannel.N_PADE_SAMPLES,0,PadeChannel.N_PADE_SAMPLES);
nevents=0

for i in range(t1041.GetEntriesFast()):
    t1041.GetEntry(i)
    for j in range(event.NPadeChan()):
        pch=event.GetPadeChan(j)
        if boardID>=0 and pch.GetBoardID()!=boardID: continue
        if chanNum>=0 and pch.GetChannelID()!=chanNum: continue
        if pch.GetMax() < minADC: continue
        if pch.GetMax() > maxADC: continue
        pch.GetHist(hw)
        nevents=nevents+1
        hw.Draw();
        if pch.GetFlags>0: pch.Dump()   # events with error flags are dumped
        c.Update()
        gSystem.Sleep(sleep);
        



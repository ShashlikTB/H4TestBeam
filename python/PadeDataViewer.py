#!/usr/bin/env python
# Simple data viewer tool.  Unpacks PADE data and displays cleanly
# Usage: python PadeDataViewer.py PADE_DATA.txt.(.bz2)
# Created 6/23/2016 B.Hirosky: Initial release

import sys, time, os, getopt
from array import array
from ROOT import *
from TBUtils import *


if len(sys.argv)<2:
    print "No PADE data file given"
    sys.exit()

nsleep=0
while 1:
    if not os.path.exists(sys.argv[1]):
        print "Waiting for file"
        time.sleep(1)
        nsleep=nsleep+1
        if nsleep==50: 
            print sys.argv[1],"not found"
            sys.exit()
    else: break

try:
    padeDat=TBOpen(sys.argv[1])
except IOError as e:
    print "Failed to open PADE data file"
    sys.exit()

padeDatFile=sys.argv[1]
print "Processing file:", padeDatFile

LoadLibs("TBLIB","libTB.so")

pch=PadeChannel()
samples=array("i",[0xFFF]*pch.__DATASIZE())
lastchannel=-1

########## main event loop ##########

while 1:
    # this is a hack to make sure don't try to 
    # read a line that is not fully written to the file
    padeline = padeDat.readline()

    if "starting spill" in padeline:
        spillNum=padeline.split()[4]
        print "Reading spill",spillNum
    if "*" in padeline: continue
    # parse PADE channel data
    (pade_ts,pade_transfer_size,pade_board_id, pade_hw_counter,
     pade_ch_number,padeEvent,waveform) = ParsePadeData(padeline)
    pch.Reset()
    for i in range(len(waveform)): samples[i]=int(waveform[i],16)
    pch.Fill(pade_ts, pade_transfer_size, pade_board_id, pade_hw_counter,
             pade_ch_number, padeEvent, samples)
    # check for data sequence error
    if (lastchannel==31): lastchannel=-1
    if (pade_ch_number-lastchannel != 1):
        pch.AddFlag(PadeChannel.kNonSequential)
    pch.Dump()

    


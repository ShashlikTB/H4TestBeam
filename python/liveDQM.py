#!/usr/bin/env python
# A Basic DQM plotter designed to run on PADE data as files are being written
# or on existing files
# Usage: python liveDQM.py PADE_DATA.txt.(.bz2)
# Created 6/28/2016 B.Hirosky: Initial release

import sys, time
from array import array
from ROOT import *
from TBUtils import *

if len(sys.argv)<2:
    print "No PADE data file given"
    sys.exit()

try:
    padeDat=TBOpen(sys.argv[1])
except IOError as e:
    print "Failed to open PADE data file"
    sys.exit()

padeDatFile=sys.argv[1]
print "Processing file:", padeDatFile

LoadLibs("TBLIB","libTB.so")


NFIELDS=130  # number of fields in a PADE chanell data line
DATALEN=533
pch=PadeChannel()
samples=array("i",[0xFFF]*pch.__DATASIZE())
x=Double()
y=Double()
z=Double()
ped=Double()
sig=Double()
logger=Logger(1000)
first=True
UPDATE_RATE=100  # Update every 100 events (per PADE board)
SLEEPTIME=5 # time to wait before checking for new data
MAXSLEEP=6  # maximum sleep times before assuming end of run 

MIN_EDGE_X=-28; MIN_EDGE_Y=-28  # detector edges in mm
MAX_EDGE_X=28;  MAX_EDGE_Y=28
mapper=Mapper.Instance();
hitsCanvas=TCanvas("tcHits","Integrated Hits Display")

# histogram to show downstream hits
hChanD=TH2F("hChanD","Channels DownStream;X [mm]; Y [mm]",
	    8,MIN_EDGE_X,MAX_EDGE_X,8,MIN_EDGE_Y,MAX_EDGE_Y)
gStyle.SetOptStat(0)

########## main event loop ##########
padeDat.seek(0,2)
eof=padeDat.tell()
padeDat.seek(0,0)  # start of file

nlines=0
nsleep=0
while 1:
    where = padeDat.tell()
    padeline = padeDat.readline()
    # this is a hack to make sure don't try to read a line that is not fully written to the file
    # if not padeline:
    if float(where)/eof>0.95:
        time.sleep(SLEEPTIME)
        padeDat.seek(0,2); eof=padeDat.tell()
        padeDat.seek(where)
        nsleep=nsleep+1
        if nsleep>MAXSLEEP: break
        continue
    else:
        nsleep=0
        if "starting" in padeline: print padeline
        nlines=nlines+1
        if len(padeline)<DATALEN or len(padeline.split()) != NFIELDS: continue
    # parse PADE channel data
    (pade_ts,pade_transfer_size,pade_board_id, pade_hw_counter,
     pade_ch_number,padeEvent,waveform) = ParsePadeData(padeline)
    pch.Reset()
    for i in range(len(waveform)): samples[i]=int(waveform[i],16)
    pch.Fill(pade_ts, pade_transfer_size, pade_board_id, pade_hw_counter,
             pade_ch_number, padeEvent, samples)
    if pch.GetFlags()>0:
        logger.Warn("Data error flagged at line:",nlines)
        pch.Dump()
        if pch.GetFlags()&PadeChannel.kCorrupt : continue
    if first:   # first good PADE channel
        mapper.SetEpoch(pch.GetTimeStamp())
        first=False
    # now get to the plotting
    channelID=pch.GetChannelID()   # boardID*100+channelNum in PADE
    mapper.ChannelXYZ(channelID,x, y, z)
    pch.GetPedestal(ped,sig)
    max = pch.GetMax()-ped
    hChanD.Fill(x, y, max)
    if padeEvent%UPDATE_RATE==0:
        hChanD.Draw("colz")
        hitsCanvas.Update()
    # continue while loop

    




#!/usr/bin/env python
# A Basic DQM plotter designed to run on PADE data as files are being written
# or on existing files
# Usage: python liveDQM.py PADE_DATA.txt.(.bz2)
# Created 6/18/2016 B.Hirosky: Initial release

import sys, time, os, getopt
from array import array
from ROOT import *
from TBUtils import *

calib={
11206	:	0.96972	,
11204	:	0.983465,
11203	:	1.15863	,
11201	:	0.918382,
11214	:	2.6449	,
11212	:	0.952073,
11211	:	0.717247,
11209	:	0.849361,
11217	:	1.03209	,
11219	:	1.06732	,
11220	:	1.01901	,
11222	:	0.898616,
11225	:	1.21219	,
11227	:	1.1689	,
11228	:	1.14172	,
11230	:	0.692681,
11200	:	0.881483,
11202	:	1.3469	,
11205	:	0.96677	,
11207	:	0.918502,
11208	:	1.10569	,
11210	:	0.949529,
11213	:	1.19399	,
11215	:	0.82994	,
11223	:	0.928523,
11221	:	0.895312,
11218	:	1.68039	,
11216	:	0.82577	,
11231	:	1.20297	,
11229	:	1.10277	,
11226	:	0.783125,
11224	:	1.01525	,
10306	:	0.768051,
10304	:	2.0169	,
10303	:	0.743468,
10301	:	1.16666	,
10314	:	0.948763,
10312	:	0.89834	,
10311	:	1.15977	,
10309	:	1.0303	,
10317	:	0.97978	,
10319	:	1.18474	,
10320	:	1.34651	,
10322	:	0.718063,
10325	:	0.859597,
10327	:	1.1133	,
10328	:	1.41161	,
10330	:	0.812996,
10300	:	1.00837	,
10302	:	0.688188,
10305	:	0.858237,
10307	:	2.56388	,
10308	:	1.02586	,
10310	:	0.952712,
10313	:	0.997399,
10315	:	1.02779	,
10323	:	1.3823	,
10321	:	0.956835,
10318	:	1.05851	,
10316	:	0.777164,
10331	:	0.716799,
10329	:	2.23926	,
10326	:	0.89099	,
10324	:	0.96526	}


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
SLEEPTIME=8      # seconds to wait before checking file for new data
MAXSLEEP=10      # maximum sleep cycles before assuming end of run 

DRAW_COPY = True
PED_MAX = 110
PED_MIN = 90
MIN_EDGE_X=-28; MIN_EDGE_Y=-28  # detector edges in mm
MAX_EDGE_X=28;  MAX_EDGE_Y=28
mapper=Mapper.Instance();

dqmCanvas=TCanvas("dqm","DQM Display")
dqmCanvas.Divide(3,2,.01,.01)
# histogram of each channel's baseline ADC
hPed   =TH2F("hPed", "Channel Pedestals;Channel Index;Pedestal",64,0,64,
             20,PED_MIN,PED_MAX)
# histogram to show which errors occur
hError =TH1F("hError","Error Flags;;Occurences",5,0,5)
hError.GetXaxis().SetBinLabel(1,"NonSeq");
hError.GetXaxis().SetBinLabel(2,"PacketCount");
hError.GetXaxis().SetBinLabel(3,"Samples");
hError.GetXaxis().SetBinLabel(4,"Saturated");
hError.GetXaxis().SetBinLabel(5,"Corrupt");
hError.GetXaxis().SetLabelSize(.06)
# histogram to show downstream hits
hChanD=TProfile2D("hChanD","Channels DownStream;X [mm]; Y [mm]",
	    8,MIN_EDGE_X,MAX_EDGE_X,8,MIN_EDGE_Y,MAX_EDGE_Y)
hmModD=TH2I();
#histogram showing the wave form of all channels
hTotalWave =TProfile2D("hTotalWave", "Waveform vs Channel;Channel Index;Sample",
                  64,0,64,100,0,100)

hw103 = TH1F()
hw112 = TH1F()

gStyle.SetOptStat(0)



########## main event loop ##########
padeDat.seek(0,2)
eof=padeDat.tell()
padeDat.seek(0,0)  # start of file

nlines=0
nsleep=0
run_complete = False
maxADC103 = 0
maxADC112 = 0
lastchannel=-1


while 1:
    # this is a hack to make sure don't try to 
    # read a line that is not fully written to the file
    if run_complete and padeDat.tell()==eof: break
    where = padeDat.tell()
    if run_complete or eof-where>800: # require some lines to start reading
        padeline = padeDat.readline()
    elif not run_complete:
        time.sleep(SLEEPTIME)
        padeDat.seek(0,2)
        eof=padeDat.tell()  # check for file update
        padeDat.seek(where)
        nsleep=nsleep+1
        if nsleep>MAXSLEEP: run_complete=True
        print "sleep",nsleep 
        continue
    nsleep=0
    if "starting" in padeline: 
        print padeline
        if DRAW_COPY:
            hChanD.Reset()
            hTotalWave.Reset()
            hPed.Reset()
    nlines=nlines+1
    if len(padeline)<DATALEN or len(padeline.split()) != NFIELDS: continue

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
    lastchannel=pade_ch_number
        
    if pch.GetFlags()>0:
        if pch.GetFlags() & PadeChannel.kNonSequential: hError.Fill(0)
        if pch.GetFlags() & PadeChannel.kPacketCount: hError.Fill(1)
        if pch.GetFlags() & PadeChannel.kSamples: hError.Fill(2)
        if pch.GetFlags() & PadeChannel.kSaturated: hError.Fill(3)
        if pch.GetFlags() & PadeChannel.kCorrupt: hError.Fill(4)
        logger.Warn("Data error flagged at line:",nlines)
        pch.Dump()
        if pch.GetFlags()&PadeChannel.kCorrupt : continue

    if first:   # first good PADE channel
        mapper.SetEpoch(pch.GetTimeStamp())
        mapper.GetModuleMap(hmModD,1);
        first=False
        
    # now get to the plotting   
    channelID=pch.GetChannelID()   # boardID*100+channelNum in PADE
    mapper.ChannelXYZ(channelID,x, y, z)
    pch.GetPedestal(ped,sig)
    
    #for plots with 0-63 id convention
    if(pade_board_id == 103):
        id = pade_ch_number
    else:
        id = pade_ch_number+32
    #Fill Pedestal Hist accounting for over/underflow
    if ped>PED_MAX:
        hPed.Fill(id,PED_MAX-.1)
    elif ped < PED_MIN:
        hPed.Fill(id,PED_MIN)
    else:
        hPed.Fill(id,ped)
    
    #finding the waveform with the highest peak
    #max = pch.GetMax()-ped
    max = (pch.GetMax()-ped)*calib[channelID]
    if maxADC103 < max and pade_board_id == 103:
        maxADC103 = max
        pch.GetHist(hw103)
    if maxADC112 < max and pade_board_id == 112:
        maxADC112 = max
        pch.GetHist(hw112)

    for i in range(len(samples)):
        hTotalWave.Fill(id,i,samples[i]-ped)

    hChanD.Fill(x, y, max)
    
    if pade_board_id==112 and pade_ch_number==31 and (padeEvent+1)%UPDATE_RATE==0:
        maxADC103 = 0
        maxADC112 = 0
        if(DRAW_COPY):
            dqmCanvas.cd(1)
            hChanD.DrawCopy("colz")
            hmModD.Draw("text same");
            dqmCanvas.Update()
            dqmCanvas.cd(2)
            hTotalWave.DrawCopy("colz")
            dqmCanvas.Update()
            dqmCanvas.cd(3)
            hPed.DrawCopy("colz")
            dqmCanvas.Update()
        else:
            dqmCanvas.cd(1)
            hChanD.Draw("colz")
            dqmCanvas.Update()
            dqmCanvas.cd(2)
            hTotalWave.Draw("colz")
            dqmCanvas.Update()
            dqmCanvas.cd(3)
            hPed.Draw("colz")
            dqmCanvas.Update()

        dqmCanvas.cd(4)
        hError.Draw("sames")
        dqmCanvas.Update()
        dqmCanvas.cd(5)
        hw103.Draw("sames")
        dqmCanvas.cd(6)
        hw112.Draw("sames")
        dqmCanvas.Update()
    # continue while loop

print "Run is probably over"
time.sleep(25)
dqmCanvas.Print(padeDatFile+"_DQM.png")
#hit_continue('Hit any key to exit')
   




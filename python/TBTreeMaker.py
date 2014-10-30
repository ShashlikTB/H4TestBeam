#!/usr/bin/env python
###############################################################################
#
# Created 04/12/2014 B.Hirosky: Initial release
# 4/30/2014: BH big cleanup and new command line args
# 7/1/2014: BH - read table positions file, if present
# 10/24/2014: BH - rewrite for H4 testbeam
###############################################################################

import os, re, glob, sys, getopt, commands
#import cProfile, pstats, StringIO
from ROOT import *
from string import split
from array import array
from TBUtils import *
from datetime import *

##### Parameter block #####

DEBUG_LEVEL = 0   # set to 0 for normal running
#DEBUG_LEVEL = 4   # set to 0 for normal running
NMAX = 1000000    # max events to process.  Note: this is approximate, b/c processing occurs by spill
NPADES=0      # counted from spill headers
MASTERID = 0  # now read from spill headers
MAXPERSPILL=1000  # do not process more that this many events per spill ( mem overwrite issue )

###########################

def usage():
    print
    print "Usage: python TBTreeMaker [OPTION] -P PADE_FILE [-B BEAM_FILE]"
    print "       A PADE file must be provided, Beam data is optional"
    print "      -n max_events  : Maximum (requested) number of events to read"
    print "                       Always reads at least 1 spill"
    print "      -v             : verbose output"
    print 
    sys.exit()



def fillTree(tree, eventDict, tbspill):  
    ndrop=0                 # counter for incomlpete events that get dropped
    if len(eventDict)==0: return
    nfill=min(len(eventDict),MAXPERSPILL)
    tree[0].SetBranchAddress("tbspill",AddressOf(tbspill))
    for ievt in range(nfill):
        if not ievt in eventDict:
            ndrop=ndrop+1
            continue
        if not eventDict[ievt].NPadeChan()==NPADES*32: 
            if DEBUG_LEVEL>0: print "Incomplete event, #PADE channels=",eventDict[ievt].NPadeChan(),NPADES
# Allow incomplete events for now, uncomment when hardware is working
#            ndrop=ndrop+1
#            continue      # only fill w/ complete events
        tree[0].SetBranchAddress("tbevent",AddressOf(eventDict[ievt]))
        tree[0].Fill()
#    sys.exit()
    return ndrop



def filler(padeDat, beamDat, NEventLimit=NMAX):

    logger=Logger(1)  # instantiate a logger, w/ 1 repetition of messages

    #=======================================================================# 
    #  Declare an element of the event class for our event                  #
    #=======================================================================#
    tbevent = TBEvent()
    tbspill = TBSpill()
    padeChannel = PadeChannel()

    #=======================================================================# 
    #  Declare new file and tree with branches                              #
    #=======================================================================#
    outFile=padeDat.replace(".bz2","").replace(".txt",".root")

    #!!! this section needs a rewrite to get beam and configuration parameters somehow
    #timeStamp=os.path.basename(outFile).replace("rec_capture_","").replace(".root","")
    #try:
    #    pdgId,momentum,gain,tableX,tableY,angle=getRunData(timeStamp)
    #except:
    #    logger.Warn("No run data found for",padeDat,"\n Either this run is not logged, or rerun getRunData.py")
    #    pdgId=0; momentum=0; gain=0; tableX=0; tableY=0; angle=0

    pdgId=0; momentum=0; gain=0; tableX=0; tableY=0; angle=0


    logger.Info("pdgId,momentum,gain,tableX,tableY,angle:",pdgId,momentum,gain,tableX,tableY,angle)
        
    fout = TFile(outFile+"_tmp", "recreate")   # write to tmp file, rename at successful close
    logger.Info("Writing to output file",outFile)

    BeamTree = [TTree("t1041", "T1041")] # ugly python hack to pass a reference
    #print "1!"
    BeamTree[0].Branch("tbevent", "TBEvent", AddressOf(tbevent), 64000, 0)
    #print "2!"
    BeamTree[0].Branch("tbspill", "TBSpill", AddressOf(tbspill), 64000, 0)
    #print "3!"

    if (NEventLimit<NMAX):
        logger.Info("Stop at end of spill after reading at least",NEventLimit,"events")
    fPade=TBOpen(padeDat)                   # open the PADE data file
    fBeam=0
    if beamDat!="": fBeam=TBOpen(beamDat)
    else: logger.Warn("No corresponding Beam data file provided")

    eventDict={} # dictionary holds event data for a spill, use event # as key
    padeDict={}  # dictionary holds PADE header data for a spill, use PADE ID as key

    lastBoardID=-1
    lastEvent=-1
    nSpills=0
    nEventsInSpill=0
    nEventsInSpillFromBeamInformation=0
    currentSpillFromBeam = 0
    nEventsTot=0
    skipToNextSpill=False
    skipToNextBoard=False
    fakeSpillData=False
    writevent=True

    # read PADE data file
    linesread=0;
    while 1:
        padeline=fPade.readline().rstrip()
        if not padeline:                                             # end of file
            ndrop=fillTree(BeamTree,eventDict,tbspill)          
            if not ndrop==0: logger.Warn(ndrop,"incomplete events dropped from tree, spill",nSpills)
            break
        linesread=linesread+1
        ###########################################################
        ############### Reading spill header information ##########

        if "starting spill" in padeline:   # new spill condition
            if nSpills>0:                  # if this is not the 1st spill, fill the tree with the spill we just read
                ndrop=fillTree(BeamTree,eventDict,tbspill)
                if not ndrop==0: logger.Warn(ndrop,"incomplete events dropped from tree, spill",nSpills)
            if (nEventsTot>=NEventLimit): 
                break

            tbspill.Reset();
            logger.Info(padeline)
            eventDict={}           # clear dictionary containing events in spill
            lastBoardID=-1         # reset counters and flags
            lastEvent=-1
            newEvent=False
            skipToNextSpill=False
            skipToNextBoard=False
            nEventsInSpill=0

            padeSpill=ParsePadeSpillHeader(padeline)
            nSpills=nSpills+1;

            tbspill.SetSpillData(padeSpill['number'],padeSpill['pcTime'],
                                 padeSpill['nTrigWC'],padeSpill['wcTime'],
                                 pdgId,momentum,tableX,tableY,angle)
            if DEBUG_LEVEL>0: 
                tbspill.Dump()
            continue  # finished w/ spill header read next line in PADE file

        # begin reading at next spill header (triggered by certain errors)
        NPADES=0
        if skipToNextSpill: continue  

        if "spill status" in padeline:   # spill header for a PADE card
            NPADES=NPADES+1
            (isMaster,boardID,status,trgStatus,
             events,memReg,trigPtr,pTemp,sTemp) = ParsePadeBoardHeader(padeline)
            tbspill.AddPade(PadeHeader(isMaster,boardID,status,trgStatus,
                                            events,memReg,trigPtr,pTemp,sTemp,gain))
            if (isMaster): MASTERID=boardID
            continue

        ############### Reading spill header information ########## 
        ###########################################################

        # parse PADE channel data
        (pade_ts,pade_transfer_size,pade_board_id,
         pade_hw_counter,pade_ch_number,padeEvent,waveform)=ParsePadeData(padeline)


        # new board/event conditions
        newBoard =  (pade_board_id != lastBoardID)
        newEvent = (padeEvent!=lastEvent)
        newMasterEvent = (pade_board_id==MASTERID and newEvent)

        if newBoard: 
            lastBoardID=pade_board_id
            lastEvent=-1
            skipToNextBoard=False
            if DEBUG_LEVEL>0: print "New board, ID=",pade_board_id
        if skipToNextBoard: continue

        # check for event overflows
        if padeEvent>MAXPERSPILL:
            if logger.Warn("PadeEvent",padeEvent,"Event count overflow in spill, reading 1st",
                        MAXPERSPILL,"events"):
                if DEBUG_LEVEL>0: logger.Info("line number",linesread)
            skipToNextSpill=True
            continue

        #print "4!"

        # check for non-sequential events
        if newEvent and (padeEvent-lastEvent)!=1:
            if logger.Warn("Nonsequential event #, delta=",padeEvent-lastEvent,
                        "this event",padeEvent,"last event",lastEvent,
                        "Board:",pade_board_id,"channel:",pade_ch_number):
                if DEBUG_LEVEL>0: logger.Info("line number",linesread)
        lastEvent=padeEvent

        # check packet counter
        goodPacketCount = (newBoard or newEvent) or (pade_hw_counter-lastPacket)==1
        if not goodPacketCount:
            if logger.Warn("Packet counter increment error, delta=",
                           pade_hw_counter-lastPacket,"Board:",pade_board_id,"channel:",pade_ch_number):
                if DEBUG_LEVEL>0: logger.Info("line number",linesread)
        lastPacket=pade_hw_counter

        #print "5!"

        # fetch ADC samples (to do: clear event from here on error)
        samples=array("i",[0xFFF]*padeChannel.__DATASIZE())
        nsamples=len(waveform)
        if nsamples != padeChannel.__DATASIZE():
            logger.Warn("Incorrect number of ADC samples, expected",
                        padeChannel.__DATASIZE(),"found:",nsamples,"Board:", pade_board_id)
            continue
        else:
            if pade_ts>=TBEvent().START_PORCH15: porch=15
            elif pade_ts>=TBEvent().END_TBEAM1: porch=32
            else: porch=0
            isSaturated = "FFF" in waveform[porch]
            if (isSaturated):
                logger.Warn("ADC shows saturation. Board:",
                            pade_board_id,"channel:",pade_ch_number,"line number",linesread)
            for i in range(nsamples): 
                samples[i]=int(waveform[i],16)

        writeChan=True   # assume channel is good to write, until proven bad

        #print "6!"

        # new event condition in master
        # NOTE: the code implicitly assumes that the MASTER is the board with LOWEST board ID
        # Boards are witten out sequentially to the data file, so this means the MASTER is always the
        # first data block
        if newMasterEvent:
            nEventsTot=nEventsTot+1
            nEventsInSpill=nEventsInSpill+1
            if padeEvent%100==0:
                print "Event in spill",padeSpill['number'],"(",padeEvent,")  / total", nEventsTot

            eventDict[padeEvent]=TBEvent()

            # search for WC spill info
            # find matching spill and event in Hodoscope file
            if fBeam!=0:
                if padeEvent==0:
                    logger.Info("Looking up beam data.  Spill:",padeSpill['number'],"Event",padeEvent)
                while 1:
                  # read file till "Event" is found
                  beamline = fBeam.readline().lstrip()
                  if not beamline: # end of file
                    logger.Warn("End of file reading beam data. nEventsInSpillFromBeamInformation=",nEventsInSpillFromBeamInformation)
                    #logger.Warn("End of file reading beam data. Wrong beam file or sync problem.")
                    fBeam=0
                    break

                  while 1:
                    if len(beamline.split()) != 0 and (beamline.split()[0]) == 'Event':    # "Event" found
                      break
                    beamline = fBeam.readline().lstrip() # read the next line

                  if "Event" in beamline: # cross check, but actually not needed
                  #if "spillNumber" in beamline:  
 
                      nEventsInSpillFromBeamInformation = nEventsInSpillFromBeamInformation+1
                      beamEvent = int(beamline.split()[1])
                      
                      beamline = fBeam.readline().rstrip()
                      beamrunNumber=int(beamline.split()[1])
                      
                      beamspill=int(beamline.split()[1])+1  # spill number starts from 1 on PADE

                      if currentSpillFromBeam != beamspill:
                        #if it is a new spill, reset the counter
                        nEventsInSpillFromBeamInformation = 0
                        currentSpillFromBeam = beamspill
                                            
                      beamline=fBeam.readline().rstrip()                        
                      beamevt = int(beamline.split()[1])
                      
                      if (beamspill != nSpills) and (beamevt != padeEvent) :
                        logger.Warn("Event number mismatch. beamspill:",beamspill,", nSpills:",nSpills," beamevt:",beamevt," padeEvent:",padeEvent )
                        continue

                      evtTimeDist = fBeam.readline()
                      evtTimeStart = fBeam.readline()
                      nEvtTimes = fBeam.readline()
                      evtTime = fBeam.readline()
                      evtTimeBoard = fBeam.readline()
                      
                      if DEBUG_LEVEL>1: print " Event:Spill ", beamEvent, " ", beamspill
                      
                      
                      # get to the "nTdcChannels"
                      tempLine = fBeam.readline().lstrip()
                      tempLine_split = tempLine.split()
                      while (tempLine_split[0] != "nTdcChannels") :
                        tempLine = fBeam.readline().lstrip()
                        tempLine_split = tempLine.split()

                      if tempLine.startswith('nTdcChannels') :
                        if DEBUG_LEVEL>2: print " >>> hodoscope wire chambers"
                        nTdcChannels = int(( tempLine_split )[1]) # --> nPatterns 12345
                        
                        tdcBoard = fBeam.readline().split() # --> patternBoard
                        tdcBoard_a = array("I",[0]*max(32,nTdcChannels))
                        for i in range(nTdcChannels):  tdcBoard_a[i] = long(tdcBoard[i+1])
                        tdcChannel = fBeam.readline().split() # --> patternChannel
                        tdcChannel_a = array("I",[0]*max(32,nTdcChannels))
                        for i in range(nTdcChannels): tdcChannel_a[i] = long(tdcChannel[i+1])

                        tdcData = fBeam.readline().split() # --> tdcData
                        if DEBUG_LEVEL>3: print "tdcData = ",tdcData
                        tdcData_a = array ("I",[0]*max(32,nTdcChannels))
                        if DEBUG_LEVEL>3: print "tdcData_a = ",tdcData_a
                        for i in range(nTdcChannels): tdcData_a[i] = long(tdcData[i+1])
                        #tdcData_a[i]=123456789
                        
                        if DEBUG_LEVEL>1: print "Found beam tdcData for spill",beamspill,"event",beamevt
                        eventDict[padeEvent].SetWireChambersData(beamrunNumber, beamspill,beamevt,tdcData_a,tdcBoard_a,tdcChannel_a,nTdcChannels)
                        
                      
                      # get to the "nPatterns"
                      tempLine = fBeam.readline().lstrip()
                      tempLine_split = tempLine.split()
                      while (tempLine_split[0] != "nPatterns") :
                        tempLine = fBeam.readline().lstrip()
                        tempLine_split = tempLine.split()

                      if tempLine.startswith('nPatterns') :
                        if DEBUG_LEVEL>1: print " >>> hodoscope "
                        nAdcChannels = int(( tempLine_split )[1]) # --> nPatterns 12345

                        pattern = fBeam.readline().split() # --> pattern
                        if DEBUG_LEVEL>1: print "pattern = ",pattern
                        pattern_a = array ("I",[0]*max(32,nAdcChannels))
                        if DEBUG_LEVEL>1: print "pattern_a = ",pattern_a
                        for i in range(nAdcChannels): pattern_a[i] = long(pattern[i+1])
                        #pattern_a[i]=123456789
                        
                        adcBoard = fBeam.readline().split() # --> patternBoard
                        adcBoard_a = array("I",[0]*max(32,nAdcChannels))
                        for i in range(nAdcChannels):  adcBoard_a[i] = long(adcBoard[i+1])
                        adcChannel = fBeam.readline().split() # --> patternChannel
                        adcChannel_a = array("I",[0]*max(32,nAdcChannels))
                        for i in range(nAdcChannels): adcChannel_a[i] = long(adcChannel[i+1])
                        
                        if DEBUG_LEVEL>1: print "Found beam adcData for spill",beamspill,"event",beamevt
                        eventDict[padeEvent].SetHodoScopeData(beamrunNumber, beamspill,beamevt,pattern_a,adcBoard_a,adcChannel_a,nAdcChannels)
                        
                      tempLine_split = fBeam.readline().rstrip().split()
                      while (tempLine_split[0] != "End" and tempLine_split[1] != "Event") :
                        tempLine_split = fBeam.readline().rstrip().split()


            #!!! This code needs to be written  
            #!!! Fetch all data from fBeam for this event

        else: # new event in a slavereadline
            if not padeEvent in eventDict:
                logger.Warn("Event number mismatch. Slave:",
                            pade_board_id,"reports event not present in master.")
                writeChan=False
          
        if writeChan:
            isLaser=(pdgId==-22)
            eventDict[padeEvent].FillPadeChannel(pade_ts, pade_transfer_size, 
                                                 pade_board_id, pade_hw_counter, 
                                                 pade_ch_number, padeEvent, samples, isLaser)
            if DEBUG_LEVEL>2: eventDict[padeEvent].GetLastPadeChan().Dump()



    #=======================================================================# 
    #  Write tree and file to disk                                          #
    #=======================================================================#
    print "Total lines read:",linesread
    print
    print "Finished processing"
    BeamTree[0].Print()
    eventsInTree=BeamTree[0].GetEntries()
    print "writing file:",outFile
    BeamTree[0].Write()
    fout.Close()
    commands.getoutput("mv -f "+outFile+"_tmp "+outFile)

    # for convinence when working interactively
    print commands.getoutput(ccat('ln -sf',outFile,' latest.root'))

    print
    logger.Info("Summary: nSpills processed= ",nSpills," Total Events Processed= ",nEventsTot)
    logger.Info("Fraction of events kept:",float(eventsInTree)/nEventsTot*100)

    logger.Summary()
    if fakeSpillData: logger.Info("Fake spill data")



if __name__ == '__main__': 
    try:
        opts, args = getopt.getopt(sys.argv[1:], "n:P:B:v")
    except getopt.GetoptError as err: usage()

    NEventLimit=NMAX
    PadeFile=""
    BeamFile=""
    verbose=false
    for o, a in opts:
        if o == "-n": NEventLimit=int(a)
        elif o == "-P": PadeFile=a
        elif o == "-B": BeamFile=a
        elif o == "-v": verbose=true

    if PadeFile=="":
        usage()

    print "Processing file(s):"
    print "PADE Data from:",PadeFile
    print "BEAM Data from:",BeamFile

    #===========================================================# 
    #  Declare data containers                                  #
    #===========================================================#
    LoadLibs("TBLIB","libTB.so")
    print "lib loaded"
    
    filler(PadeFile,BeamFile,NEventLimit)

    print "Exiting" 
    exit(0)

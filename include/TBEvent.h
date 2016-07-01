//Created 4/12/2014 B.Hirosky: Initial release

#ifndef TBEVENT_H
#define TBEVENT_H
#include "PadeChannel.h"
#include "Hodoscope.h"
#include "Mapper.h"
#include <vector>
#include <algorithm>

using std::vector;

class PadeHeader : public TObject{
  ClassDef(PadeHeader,1); 
 public:
  PadeHeader(){;}
  PadeHeader(Bool_t master, UShort_t board, UShort_t stat,
	    UShort_t tstat, UShort_t events, UShort_t mreg,
	    UShort_t pTrg, UShort_t pTmp, UShort_t sTmp, UShort_t gain):
  _isMaster(master), _boardID(board), _status(stat), _trgStatus(tstat), 
    _events(events), _memReg(mreg), _trigPtr(pTrg), 
    _pTemp(pTmp), _sTemp(sTmp), _gain(gain), _bias(0){;}
  
  /// PADE gain in packed format
  /** 
      Packing format LNA [bits 1:0]  PGA [bits 3:2]  VGA [bits 15:4]
  **/
  UShort_t Gain() const {return _gain;}
  UShort_t BoardID() const {return _boardID;}
  UShort_t Events() const {return _events;}
  Bool_t IsMaster() const {return _isMaster;}
  UShort_t PadeTemp() const {return _pTemp;}
  UShort_t SipmTemp() const {return _sTemp;}

 private:
  Bool_t _isMaster;
  UShort_t _boardID;
  UShort_t _status;
  UShort_t _trgStatus;
  UShort_t _events;
  UShort_t _memReg;
  UShort_t _trigPtr;
  UShort_t _pTemp;    ///< temperature on PADE board
  UShort_t _sTemp;    ///< temperature on SIPM board
  UShort_t _gain;     ///< LNA [bits 1:0]  PGA [bits 3:2]  VGA [bits 15:4]
  UShort_t _bias;     ///< main bias setting
};



/// Container for spill-related data
/**
   Container for spill-related data
   Beam types are given usin PDG ID's
   11 : electron
   -11 : positron
   12 : muon
   211 : pion
   2212 : proton
   -22 : Laser
**/
class TBSpill : public TObject {
  ClassDef(TBSpill,1);  // Spill header info
 public:
 TBSpill(Int_t spillNumber=0, ULong64_t pcTime=0, Int_t nTrigWC=0, ULong64_t wcTime=0, 
	 Int_t pdgID=0, Float_t nomMomentum=0,
	 Float_t tableX=-999, Float_t tableY=-999, Float_t angle=0, 
	 Float_t boxTemp=0, Float_t roomTemp=0) : 
  _spillNumber(spillNumber), _pcTime(pcTime), 
    _nTrigWC(nTrigWC), _wcTime(wcTime), _pdgID(pdgID), _nomMomentum(nomMomentum),
    _tableX(tableX), _tableY(tableY), _angle(angle), 
    _boxTemp(boxTemp), _roomTemp(roomTemp) {;}
  Int_t GetSpillNumber() const {return _spillNumber;}
  ULong64_t GetPCTime() const {return _pcTime;}
  Int_t GetnTrigWC() const {return _nTrigWC;}
  ULong64_t GetWCTime() const {return _wcTime;}
  Float_t GetTableX() const {return _tableX;}
  Float_t GetTableY() const {return _tableY;}
  Float_t GetAngle() const {return _angle;}
  Int_t GetPID() const {return _pdgID;}
  Float_t GetMomentum() const {return _nomMomentum;}
  Int_t NPades() const {return _padeHeader.size();}

  void SetTableX(float tableX) { _tableX = tableX; }
  void SetTableY(float tableY) { _tableY = tableY; }
  void SetMomentum(float nomMomentum) { _nomMomentum = nomMomentum;}
  
  void Dump() const;
  /// index 0:n-1 
  PadeHeader const* GetPadeHeader(Int_t i) {
    if (i<NPades()) return &(_padeHeader[i]);
    return 0;
  }
  // setters
  void Reset();
  void SetSpillData(Int_t spillNumber, ULong64_t pcTime, Int_t nTrigWC, ULong64_t wcTime,
		    Int_t pdgID=0, Float_t nomMomentum=0, 
		    Float_t tableX=-999, Float_t tableY=-999, Float_t angle=0,
		    Float_t boxTemp=0, Float_t roomTemp=0);
  void SetSpillNumber(Int_t s) {_spillNumber=s;}
  void SetPCTime(ULong64_t t) {_pcTime=t;}
  //void SetnTrigWC(Int_t n) {_nTrigWC=n;}
  //void SetWCTime(ULong64_t t) {_wcTime=t;}
  void AddPade(PadeHeader pade){_padeHeader.push_back(pade);}
 private:
  Int_t         _spillNumber;              ///< spill # counted by PADE
  ULong64_t     _pcTime;                   ///< spill time stamp from PC
  Int_t         _nTrigWC;                  ///< triggers reported by WC
  ULong64_t     _wcTime;                   ///< WC time read by PADE PC  
  vector<PadeHeader> _padeHeader;
  // beam and detector parameters
  Int_t         _pdgID;                    ///< particle ID for beam
  Float_t       _nomMomentum;              ///< beam momentum setting
  Float_t       _tableX;                   ///< table position
  Float_t       _tableY;                   ///< table position
  Float_t       _angle;                    ///< table angle
  Float_t       _boxTemp;                  ///< temperature in environmental box
  Float_t       _roomTemp;                 ///< temperature in test beam area
};

/// Storage container for raw data from test beam
class TBEvent : public TObject {
  ClassDef(TBEvent,1);  //Event structure
 public:
  enum TBRun { 
    TBRun1=0,   ///< April 2014
    TBRun2a=1,  ///< Start of July-Aug 2014 run (32 ADC samples used for porch)
    TBRun2b=2,  ///< Final July-Aug 2014 cfg. (15 ADC samples used for porch)
    TBRun2c=3,  ///< More precise WC time stamps in PADE DAQ
    TBUndef=10
  };
  /// earliest TB data run
  static const ULong64_t START_TBEAM1=635321637512389603L;   
  /// end of TBRun1
  static const ULong64_t END_TBEAM1=635337576077954884L;      
  /// beginning of TBRun2b
  static const ULong64_t START_PORCH15=635421671607690753L;   
  /// More precise end of spill time reported
  /** Mod of PADE WC Spill time stamp.  Previously, reported end of spill + PADE RO time.
      Now end of spill time reported.  This is 0-1 seconds behind spill time reported by WC DAQ **/
  static const ULong64_t START_NEWWCSYNC=635432861909176340L;  
  static const ULong64_t END_TBEAM2=635440566331915360L;
  /// Swap out board 117 for board 16, preparing for H4 TB
  static const ULong64_t START_H4TB=635479530091849620L;  // before installation
  static const ULong64_t H4TB_MAP1=635502232652093170L;   // first data configuration 29-Oct-2014
  static const ULong64_t H4TB_MAP2=635502992419392830L;   // 2nd data configuration 30-Oct-2014
  static const ULong64_t H4TB_MAP3=635503713451583370L;   // 3rd data configuration 31-Oct-2014
  static const ULong64_t H4TB_MAP4=635504663348032610L;   // 4th data configuration 01-Nov-2014
  static const ULong64_t H4TB_MAP5=635530000000000000L;   // 5th data configuration 08-May-2015
  static const ULong64_t H4TB_MAP6=635678514000000000L;   // 6th data configuration 21-May-2015
  static const ULong64_t H4TB_MAP7=636011699058128660L;   // 7th data configuration 09-Jun-2016


  /// Times when a change in pulse shapes occur
  /** Times of changes in PADE configuration that caused changes in pulse shapes:
      31-Jul-2014 23:45 
      11-Aug-2014 11:10
      14-Aug-2014 00:00
      16-Aug-2014 14:15
      Timestamp in .NET format is 100ns ticks from Jan 1, 0001 00:00:00
      It is calculated using universal unix time (seconds since Jan 1, 1970 00:00:00) using
      timestamp = unixtime * 10000000 + 621355968000000000
  **/
  static const ULong64_t PULSESHAPE_T1=635424471000000000L;  
  static const ULong64_t PULSESHAPE_T2=635433522000000000L;  
  static const ULong64_t PULSESHAPE_T3=635435712000000000L;  
  static const ULong64_t PULSESHAPE_T4=635437953000000000L;  
  static const ULong64_t PULSESHAPE_T5=636029280000000000L;  

  // event flags
  /// Pade Channel error flags
  static const UInt_t F_SATURATED=1;  /// flag for ADC saturation 0xFFF 
  static const UInt_t F_NOTIME=2;
  static const UInt_t F_FRAGMENT=4;
  static const UInt_t F_CORRUPT=8;   /// flag for data corruption ADC>0xFFF
  TBEvent();

  ~TBEvent(){};
  
  void Reset();    // clear data

  // getters (tbd - return (const) references, not copies, where appopriate)
  Int_t NPadeChan() const {return padeChannel.size();}  // total number of channels
  Int_t NPadeChan(UInt_t boardID); // number of channels associated w/ boardID
  PadeChannel GetPadeChan(const int idx) const {return padeChannel[idx];}
  PadeChannel GetLastPadeChan() const {return padeChannel.back();}
  Hodoscope GetHSChan() {return hodoscope;}
  static TBRun GetRunPeriod(ULong64_t padeTime);
  TBRun GetRunPeriod() const;
  ULong64_t GetTimeStamp();  // time stamp from 1st PADE channel
  UInt_t GetErrorFlags() const {return _flags;}
  /// check if data is present for boardID
  /// return index of first channel for this board
  int FindBoard(UInt_t boardID); 
  Int_t GetTime() {return _time;}
  
  // setters
  void SetErrorFlags(UInt_t flags) {_flags=flags;}
  void AddErrorFlags(UInt_t flags) {_flags|=flags;}
  void SetTime(Int_t time) {_time=time;}
  void SetPadeChannel(const PadeChannel p, Int_t i) {padeChannel[i]=p;}
  UInt_t FillPadeChannel(ULong64_t ts, UShort_t transfer_size, 
		       UShort_t  board_id, UInt_t hw_counter, 
		       UInt_t ch_number,  UInt_t eventnum, Int_t *wform,
		       Bool_t isLaser=false);
  void SetHodoScopeData   (Int_t runNumber, Int_t spillNumber, Int_t eventNum,  unsigned int *adcData,  unsigned int *adcBoard,  unsigned int * adcChannel, Int_t nAdcChannels);
  void SetWireChambersData(Int_t runNumber, Int_t spillNumber, Int_t eventNum,  unsigned int *tdcData,  unsigned int *tdcBoard,  unsigned int * tdcChannel, Int_t nTdcChannels);
  
  void SetHodoScope (Hodoscope inhodoscope) { hodoscope = inhodoscope; }
  void SortPadeChannels(){std::sort(padeChannel.begin(),padeChannel.end());}
  private:
  std::vector<PadeChannel> padeChannel;
  Hodoscope hodoscope;  // the hodoscope data
  UInt_t _flags;
  Int_t _time;  /// event time in useconds from 1st in spill (-1 is unknown)
};


#endif

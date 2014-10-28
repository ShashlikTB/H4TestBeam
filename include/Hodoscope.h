#ifndef HODOSCOPE_H 
#define HODOSCOPE_H

#include "TObject.h"

// #define nPlanesHodo 4
// #define nFibersHodo 64

// #define nPlanesSmallHodo 2
// #define nFibersSmallHodo 8

#define hodoX1 0
#define hodoY1 1
#define hodoX2 2
#define hodoY2 3

// static const UInt_t MaxTdcChannels = 4;
static const UInt_t MaxTdcChannels = 32;
static const UInt_t MaxTdcReadings = 20;



//schema to be checked: Xleft, Xright, Ydown, Yup
#define wcXl 0
#define wcXr 1
#define wcYd 3
#define wcYu 2

#include <algorithm>

class Hodoscope : public TObject {
  ClassDef(Hodoscope,1); 
  
 public:
  Hodoscope();
  ~Hodoscope();
  void SetSpill(Int_t spillNum){_spillNum=spillNum;};
  void SetEvent(Int_t eventNum){_eventNum=eventNum;};
  void SetADCData(unsigned int *adcData, unsigned int * adcBoard, unsigned int *adcChannel, Int_t nAdcChannels);
  void SetTDCData(unsigned int *tdcData, unsigned int * tdcBoard, unsigned int *tdcChannel, Int_t nTdcChannels);
   
  
  void FillHodo();
  void FillFiberOrder();
  
  void FillWC();
   
  Int_t GetSpill() const {return _spillNum;};
  Int_t GetEvent() const {return _eventNum;} ;

  
  void Reset();
  void Dump() const;

  // private:
  Int_t     _spillNum;
  Int_t     _eventNum;
  ULong_t   _evtTime[3];
  unsigned int  _adcData[32]; //---- pattern
  unsigned int  _adcBoard[32]; //---- patternBoard
  unsigned int  _adcChannel[32]; //---- patternChannel
//   UShort_t  _adcBoard[32]; //---- patternBoard
//   UShort_t  _adcChannel[32]; //---- patternChannel
  unsigned int     _nAdcChannels;
  
  bool _fibersOn[4][64];
//   bool _fibersOn[nPlanesHodo][nFibersHodo];
  
  
  
  //---- wire chambers
  unsigned int _nTdcChannels;
  unsigned int  _tdcBoard[MaxTdcChannels];
  unsigned int  _tdcChannel[MaxTdcChannels];
  unsigned int  _tdcData[MaxTdcChannels];
    
    
    
  //tdc readings
  std::vector<float> _tdc_readings[MaxTdcChannels];
  float _tdc_recox, _tdc_recoy;
  
  
  std::vector<int> _fiberOrderA;
  std::vector<int> _fiberOrderB;
  
};

#endif

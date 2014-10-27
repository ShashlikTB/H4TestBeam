#ifndef HODOSCOPE_H 
#define HODOSCOPE_H

#include "TObject.h"


#define nPlanesHodo 4
#define nFibersHodo 64

#define hodoX1 0
#define hodoY1 1
#define hodoX2 2
#define hodoY2 3



class Hodoscope : public TObject {
  ClassDef(Hodoscope,1); 
 public:
  Hodoscope();
  void SetSpill(Int_t spillNum){_spillNum=spillNum;}
  void SetEvent(Int_t eventNum){_eventNum=eventNum;}
  void SetADCData(unsigned int *adcData, unsigned int * adcBoard, unsigned int *adcChannel, Int_t nAdcChannels);
  
  
  void FillHodo();
  void FillFiberOrder();
  
  Int_t GetSpill() const {return _spillNum;}
  Int_t GetEvent() const {return _eventNum;} 

  
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
  
  bool _fibersOn[nPlanesHodo][nFibersHodo];
  //   unsigned int _patternChannel[16]; //MAX_PATTERNS 16
  
  std::vector<int> _fiberOrderA;
  std::vector<int> _fiberOrderB;
  
};

#endif

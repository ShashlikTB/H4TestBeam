#ifndef HODOSCOPE_H 
#define HODOSCOPE_H

#include "TObject.h"

class Hodoscope : public TObject {
  ClassDef(Hodoscope,1); 
 public:
  Hodoscope();
  void SetSpill(Int_t spillNum){_spillNum=spillNum;}
  void SetEvent(Int_t eventNum){_eventNum=eventNum;}
  void SetADCData(Int_t *adcData);
  Int_t GetSpill() const {return _spillNum;}
  Int_t GetEvent() const {return _eventNum;} 

  void Reset();
  void Dump() const;

  // private:
  Int_t     _spillNum;
  Int_t     _eventNum;
  ULong_t   _evtTime[3];
  UShort_t  _adcData[32];
};

#endif

#ifndef HODOSCOPE_H 
#define HODOSCOPE_H

#include "TObject.h"

class Hodoscope : public TObject {
  ClassDef(Hodoscope,1); 
 public:
  Hodoscope();
  void SetSpill(Int_t spill);
  void SetEvent(Int_t event);
  Int_t GetSpill() const {return _spill;}
  Int_t GetEvent() const {return _event;} 

  void Reset();
  void Dump() const;

  // private:
  Int_t     _spill;
  Int_t     _event;
};

#endif

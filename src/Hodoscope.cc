#include "Hodoscope.h"
#include <iostream>
using std::cout;
using std::endl;

Hodoscope::Hodoscope(){
  Reset();
}

void Hodoscope::Reset(){
  _spillNum=-1;
  _eventNum=-1;
  for (int i=0; i<32;i++) _adcData[i]=0;
}

void Hodoscope::SetADCData(Int_t *adcData){
  for (int i=0; i<32;i++) _adcData[i]=adcData[i];
}

void Hodoscope::Dump() const{
  cout << "Hodoscope Data:: Spill " <<  _spillNum << " Event " 
       << _eventNum << endl << "adcData ";
  for (int i=0; i<32;i++) cout << _adcData[i] << " ";
  cout << endl;
}



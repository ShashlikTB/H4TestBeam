#include "Hodoscope.h"
#include <iostream>
using std::cout;
using std::endl;

Hodoscope::Hodoscope(){
  Reset();
  FillFiberOrder(); //---- once and for all
}

Hodoscope::~Hodoscope(){
}

void Hodoscope::Reset(){
  _spillNum=-1;
  _eventNum=-1;
  for (int i=0; i<32;i++) {
    _adcChannel[i] = 99;
    _adcBoard[i]   = 0;
    _adcData[i]    = 0;
  }
  
  for (unsigned int j=0; j<MaxTdcChannels; j++){
   _tdc_readings[j].clear();
  }
  _tdc_recox=-999;
  _tdc_recoy=-999;
  
}

void Hodoscope::SetADCData(unsigned int *adcData, unsigned int * adcBoard, unsigned int *adcChannel, Int_t nAdcChannels){
 _nAdcChannels = nAdcChannels;
 for (int i=0; i<32; i++) _adcBoard[i]   = adcBoard[i];
 for (int i=0; i<32; i++) _adcChannel[i] = adcChannel[i];
 for (int i=0; i<32; i++) _adcData[i] = adcData[i];
 
 FillHodo();
}


void Hodoscope::SetTDCData(unsigned int *tdcData, unsigned int * tdcBoard, unsigned int *tdcChannel, Int_t nTdcChannels){
 _nTdcChannels = nTdcChannels;
 for (int i=0; i<32; i++) _tdcBoard[i]   = tdcBoard[i];
 for (int i=0; i<32; i++) _tdcChannel[i] = tdcChannel[i];
 for (int i=0; i<32; i++) _tdcData[i] = tdcData[i];
 
 FillWC();
}



void Hodoscope::Dump() const{
 std::cout << "Hodoscope Data:: Spill " <<  _spillNum << " Event " << _eventNum << std::endl << "adcChannel :: adcData ";
  for (int i=0; i<32;i++) std::cout << _adcChannel[i] << " :: " << _adcData[i] << " ";
  std::cout << std::endl;
  
  for (int ipos = 0; ipos<4; ipos++) {
   std::cout << " ipos = " << ipos << std::endl;
   for (unsigned int j=0; j<32; j++){
    std::cout << " " << _fibersOn[ipos][j]; // FIXME j
   }
   std::cout << std::endl;
  }
  
}




void Hodoscope::FillHodo(){
 
//  std::cout << " Hodoscope::FillHodo " << std::endl;
//  
//  for(int i=0; i<nPlanesHodo; ++i){
//   for(int j=0; j<nFibersHodo; ++j){
//    _fibersOn[i][j] = 0;
//   }
//  }
 
 for(int i=0; i<4; ++i){
  for(int j=0; j<64; ++j){
   _fibersOn[i][j] = 0;
  }
 }

 
 
 
 for(unsigned int i=0;i<_nAdcChannels;++i){
  
  if(_adcBoard[i]==0x08030001 || _adcBoard[i]==0x08030002){
   
   int pos = -1; // here is where the real hodoscope mapping is done
   if (_adcBoard[i]==0x08030001){
    pos = (_adcChannel[i]<2) ? hodoY2 : hodoX2;
   }
   else if (_adcBoard[i]==0x08030002){
    pos = (_adcChannel[i]<2) ? hodoY1 : hodoX1;
   }
   std::vector<int> *fiberorder = (bool)( _adcChannel[i]&0b1) ? &_fiberOrderB : &_fiberOrderA;
   
   for (unsigned int j=0; j<32; j++){
    bool thisfibon = (_adcData[i]>>j)&0b1;
//     std::cout << " j = " << j << " thisfibon = " << thisfibon  << " [fiberorder->size() = " << fiberorder->size() <<   "]" << std::endl;
//     std::cout << " fiberorder->size() = " << fiberorder->size() << std::endl;
    if (fiberorder->size() >= j) { //---- FIXME check
     _fibersOn[pos][fiberorder->at(j)-1] = thisfibon;
    }
   }
   
  }
    
 }
 
}





void Hodoscope::FillWC(){
 
 for (unsigned int i=0; i<_nTdcChannels; i++){
  if (_tdcBoard[i]==0x07030001 && _tdcChannel[i]<MaxTdcChannels){
   _tdc_readings[_tdcChannel[i]].push_back((float)_tdcData[i]);
  }
 }
 if (_tdc_readings[wcXl].size()!=0 && _tdc_readings[wcXr].size()!=0){
  float TXl = *std::min_element(_tdc_readings[wcXl].begin(),_tdc_readings[wcXl].begin()+_tdc_readings[wcXl].size());
  float TXr = *std::min_element(_tdc_readings[wcXr].begin(),_tdc_readings[wcXr].begin()+_tdc_readings[wcXr].size());
  float X = (TXr-TXl)*0.005; // = /40./5./10. //position in cm 0.2mm/ns with 25ps LSB TDC
  _tdc_recox = X;
 }
 if (_tdc_readings[wcYd].size()!=0 && _tdc_readings[wcYu].size()!=0){
  float TYd = *std::min_element(_tdc_readings[wcYd].begin(),_tdc_readings[wcYd].begin()+_tdc_readings[wcYd].size());
  float TYu = *std::min_element(_tdc_readings[wcYu].begin(),_tdc_readings[wcYu].begin()+_tdc_readings[wcYu].size());
  float Y = (TYu-TYd)*0.005; // = /40./5./10. //position in cm 0.2mm/ns with 25ps LSB TDC
  _tdc_recoy = Y;
 }
 
}





//---- from H4DQM/src/plotterTools.cpp
void Hodoscope::FillFiberOrder(){

 _fiberOrderA.clear();
 _fiberOrderB.clear();

 
 _fiberOrderA.push_back(31);
 _fiberOrderA.push_back(29);
 _fiberOrderA.push_back(23);
 _fiberOrderA.push_back(21);
 _fiberOrderA.push_back(5);
 _fiberOrderA.push_back(7);
 _fiberOrderA.push_back(15);
 _fiberOrderA.push_back(13);
 _fiberOrderA.push_back(1);
 _fiberOrderA.push_back(3);
 _fiberOrderA.push_back(11);
 _fiberOrderA.push_back(9);
 _fiberOrderA.push_back(6);
 _fiberOrderA.push_back(8);
 _fiberOrderA.push_back(16);
 _fiberOrderA.push_back(14);
 _fiberOrderA.push_back(17);
 _fiberOrderA.push_back(27);
 _fiberOrderA.push_back(19);
 _fiberOrderA.push_back(25);
 _fiberOrderA.push_back(24);
 _fiberOrderA.push_back(22);
 _fiberOrderA.push_back(32);
 _fiberOrderA.push_back(30);
 _fiberOrderA.push_back(4);
 _fiberOrderA.push_back(2);
 _fiberOrderA.push_back(12);
 _fiberOrderA.push_back(10);
 _fiberOrderA.push_back(20);
 _fiberOrderA.push_back(18);
 _fiberOrderA.push_back(28);
 _fiberOrderA.push_back(26);
 
 _fiberOrderB.push_back(54);
 _fiberOrderB.push_back(64);
 _fiberOrderB.push_back(56);
 _fiberOrderB.push_back(62);
 _fiberOrderB.push_back(49);
 _fiberOrderB.push_back(51);
 _fiberOrderB.push_back(59);
 _fiberOrderB.push_back(57);
 _fiberOrderB.push_back(53);
 _fiberOrderB.push_back(55);
 _fiberOrderB.push_back(63);
 _fiberOrderB.push_back(61);
 _fiberOrderB.push_back(45);
 _fiberOrderB.push_back(47);
 _fiberOrderB.push_back(37);
 _fiberOrderB.push_back(39);
 _fiberOrderB.push_back(34);
 _fiberOrderB.push_back(42);
 _fiberOrderB.push_back(36);
 _fiberOrderB.push_back(44);
 _fiberOrderB.push_back(50);
 _fiberOrderB.push_back(52);
 _fiberOrderB.push_back(58);
 _fiberOrderB.push_back(60);
 _fiberOrderB.push_back(38);
 _fiberOrderB.push_back(48);
 _fiberOrderB.push_back(40);
 _fiberOrderB.push_back(46);
 _fiberOrderB.push_back(41);
 _fiberOrderB.push_back(43);
 _fiberOrderB.push_back(33);
 _fiberOrderB.push_back(35);

}

#include "Hodoscope.h"
#include <iostream>
using std::cout;
using std::endl;

Hodoscope::Hodoscope(){
  Reset();
}

void Hodoscope::Reset(){
  _spill=-1;
  _event=-1;
}

void Hodoscope::Dump() const{
  cout << "Hodoscope Data:: Spill " <<  _spill << " Event " 
       << _event << endl;
}



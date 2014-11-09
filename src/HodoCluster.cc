#include "HodoCluster.h"
#include <iostream>

//---- Origin: https://github.com/CeF3TB/H4_2014/blob/master/Analysis/src/HodoCluster.cc
//---- kindly provided by CeF3TB group
//---- Author: Francesco Pandolfi


void HodoCluster::addFibre( int i ) {
 
 float xmin = 0.5*(hodoTotFibres_-1)*fibreWidth_;
 float thisPos = fibreWidth_*((float)i) - xmin;
 
 if( size_==0 ) {
  
  pos_ += thisPos;
  size_+=1;
  
 } else {
  
  pos_ *= (float)size_;
  pos_ += thisPos;
  size_+=1;
  pos_ /= size_;
  
 }
 
}
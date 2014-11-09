#ifndef HodoCluster_h
#define HodoCluster_h

//---- Origin: https://github.com/CeF3TB/H4_2014/blob/master/Analysis/interface/HodoCluster.h
//---- kindly provided by CeF3TB group
//---- Author: Francesco Pandolfi

class HodoCluster {
 
public:
 
 HodoCluster( int totFibres, float width ) {
  size_ = 0;
  pos_ = 0.;
  hodoTotFibres_ = totFibres;
  fibreWidth_ = width;
 }
 
 ~HodoCluster() {};
 
 int getSize() { return size_; };
 float getPosition() { return pos_; };
 
 void addFibre( int i );
 
 
private:
 
 int size_;
 float pos_;
 
 int hodoTotFibres_;
 float fibreWidth_; // in mm
 
};


#endif

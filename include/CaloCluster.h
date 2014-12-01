#ifndef CaloCluster_h
#define CaloCluster_h

//---- Author: Andrea Massironi

//---- position units here is "mm"
//---- energy   units here is "GeV"


#include "Mapper.h"
#include "TBRecHit.h"


class CaloCluster {
 
public:
 
 CaloCluster() {
  Reset();
  _mapper = Mapper::Instance();
  _w0 = 5.0;
 }
 
 ~CaloCluster() {};
 
 void Reset();
 float getPositionX() { return _pos_x; };
 float getPositionY() { return _pos_y; };
 std::pair<float, float> getPosition() { return std::pair<float, float>(_pos_x, _pos_y); };
 float getEnergy() { return _energy; };

 std::vector<float> getCaloClusterComponents() { return _vector_energies; };
 
 float DR (float x1, float x2, float y1, float y2);  
 void doCalorimeterReconstruction( std::vector<TBRecHit>* rechits, int face, float maxDR, int fiberLevel = 0);
 
 void setW0(float w0) { _w0 = w0; };
 void setMapperEpoch(ULong64_t timestamp) { _mapper->SetEpoch(timestamp); };
 
 
 
 
private:
 
 float _pos_x;
 float _pos_y;
 float _energy;
  
 std::vector <float> _vector_energies;
 
 Mapper* _mapper;
 
 float _w0;
};


#endif

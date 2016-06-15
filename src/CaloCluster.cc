#include "CaloCluster.h"
#include <iostream>

//---- Author: Andrea Massironi

//---- position units here is "mm"
//---- energy   units here is "GeV"





//---- distance definition in calorimeter position
void CaloCluster::Reset () {
 _pos_x = 0.;
 _pos_y = 0.;
 _energy = 0.; 
}


void CaloCluster::setInterCalibrationConstants(std::string nameFile){   
  std::ifstream file (nameFile.c_str()); 
  std::string buffer;
  int num;
  float value;
  
  std::cout << " InterCalibration Constants : " << nameFile << std::endl;
  if(!file.is_open()){
   std::cerr << "** ERROR: Can't open '" << nameFile << "' for input InterCalibration Constants" << std::endl;
  }
  else {
   while(!file.eof()) {
    getline(file,buffer);
    if (buffer != "" && buffer.at(0) != '#'){ ///---> save from empty line at the end!
     std::stringstream line( buffer );      
     line >> num; 
     line >> value; 
     _intercalibration_constants[num] = value;
     std::cout << " " << num << " :: " << value << std::endl;
    } 
   }
  }
 
}
 
 




void CaloCluster::setRecHits(std::vector<TBRecHit>* rechits){
 _rechits.clear();
 for (UInt_t j = 0; j < rechits->size(); j++){
  _rechits.push_back(rechits->at(j));
 }
}



//---- distance definition in calorimeter position
float CaloCluster::DR (float x1, float x2, float y1, float y2) {
 return sqrt((x1-x2)*(x1-x2) + (y1-y2)*(y1-y2));
}


void CaloCluster::doCalorimeterReconstruction(int face, float maxDR, int fiberLevel){
 //----      adc              X      Y
 std::map < float, std::pair<float, float> > map_of_calo_clusters;
 float max;
 int channelID;
 
 //---- module level
 if (fiberLevel == 0) {
  for (UInt_t j = 0; j < _rechits.size(); j++){
   TBRecHit &hit=_rechits.at(j);
   //   ped = hit.Pedestal();
   //   sig = hit.NoiseRMS();
   max = hit.AMax();
   //   maxTime = hit.TRise();
   channelID = hit.GetChannelID();
   
   if (max<0.1) continue; //---- very low threshold, just to remove noisy noise
   
   int moduleID,fiberID;
   _mapper->ChannelID2ModuleFiber(channelID,moduleID,fiberID);  // get module and fiber IDs
   
//    std::cout << " moduleID = " << moduleID << std::endl;
   
   double x,y;
   if (fiberLevel == 0) _mapper->ModuleXY(moduleID,x,y);
   else                 _mapper->FiberXY(fiberID, x, y);
   
   //---- only the wanted face (front [1] or back [-1])
   if (moduleID<0 && face>0) {
    continue;
   }
   if (moduleID>0 && face<0) {
    continue;
   }
   
   //---- apply module intercalibration
   if (_intercalibration_constants.find( moduleID ) != _intercalibration_constants.end()) {
    max = max * _intercalibration_constants[moduleID];
   }
      
   
   
   std::pair<float, float> xy_pair;
   xy_pair.first = x;
   xy_pair.second = y;
   
   //---- check if that module was already saved
   //---- if yes, then sum the energy
   float old_energy = 0;
   for(std::map < float, std::pair<float, float> >::iterator iterator = map_of_calo_clusters.begin(); iterator != map_of_calo_clusters.end(); iterator++) {    
    if (iterator->second.second == y && iterator->second.first == x ) {
     float temp_energy = iterator->first;
     map_of_calo_clusters.erase(temp_energy);
     old_energy = -temp_energy;
     break; //---- not possible more than one time per module by construction
    }
   }
   
   max = max + old_energy;
   map_of_calo_clusters[-max] = xy_pair;
  }
  
  //   std::cout << " size = " << map_of_calo_clusters.size() << std::endl;
  
 }
 
 
 //---- fiber level
 if (fiberLevel == 1) {
  
  for (UInt_t j = 0; j < _rechits.size(); j++){
   TBRecHit &hit=_rechits.at(j);
   //   ped = hit.Pedestal();
   //   sig = hit.NoiseRMS();
   max = hit.AMax();
   //   maxTime = hit.TRise();
   channelID = hit.GetChannelID();
   
   if (max<0.1) continue; //---- very low threshold, just to remove noisy noise
   
   int moduleID,fiberID;
   _mapper->ChannelID2ModuleFiber(channelID,moduleID,fiberID);  // get module and fiber IDs
   
   double x,y;
   if (fiberLevel == 0) _mapper->ModuleXY(moduleID,x,y);
   else                 _mapper->FiberXY(fiberID, x, y);
   
   //---- only the wanted face (front [1] or back [-1])
   if (moduleID<0 && face>0) {
    continue;
   }
   if (moduleID>0 && face<0) {
    continue;
   }
   
   std::pair<float, float> xy_pair;
   xy_pair.first = x;
   xy_pair.second = y;
   
   map_of_calo_clusters[-max] = xy_pair;
  }
  
 }
 
 //---- do clustering ----
 int num_clusters = 0;
 float x_cluster_logE = 0;
 float y_cluster_logE = 0;
 float weight_cluster = 0;
 float energy_cluster = 0;
 
 float x_cluster_max = -1000;
 float y_cluster_max = -1000;
 
 
 //  std::cout << " _vector_energies.size() = " << _vector_energies.size() << std::endl;
 _vector_energies.clear();
 
 //---- first calculate cluster energy
 for( std::map < float, std::pair<float, float> >::iterator ii=map_of_calo_clusters.begin(); ii!=map_of_calo_clusters.end(); ii++) {
  //   std::cout << " energy = " << - ii->first << std::endl;
  if (x_cluster_max == -1000) {
   x_cluster_max = (ii->second.first); //---- seed position X
   y_cluster_max = (ii->second.second); //---- seed position X   
   float energy_temp = - ii->first;
   energy_cluster = energy_cluster + energy_temp;
   _vector_energies.push_back(energy_temp);
   num_clusters++;
  }
  else {
   if (DR(ii->second.first, x_cluster_max, ii->second.second, y_cluster_max) < maxDR) {
    float energy_temp = - ii->first;
    energy_cluster = energy_cluster + energy_temp;
    _vector_energies.push_back(energy_temp);
    num_clusters++;
   }
  }
 }
 
 //  std::cout << " num_clusters = " << num_clusters << " energy_cluster = " << energy_cluster << std::endl;
 num_clusters = 0;
 
 //---- then calculate position 
 //  std::cout<< " new cluster " << std::endl;
 if (energy_cluster > 0) {
  for( std::map < float, std::pair<float, float> >::iterator ii = map_of_calo_clusters.begin(); ii != map_of_calo_clusters.end(); ii++) {
   //   std::cout << " energy = " << - ii->first << std::endl;
   if (DR(ii->second.first, x_cluster_max, ii->second.second, y_cluster_max) < maxDR) {
    float energy_temp = - ii->first;
    float wi = (_w0 + log(energy_temp/energy_cluster));
    if (wi > 0) {
     x_cluster_logE = x_cluster_logE + (ii->second.first)  * wi ;
     y_cluster_logE = y_cluster_logE + (ii->second.second) * wi ;
     weight_cluster = weight_cluster + wi;
     num_clusters++;
    }
   }
  }
 }
 
 //  std::cout << " after num_clusters = " << num_clusters << std::endl;
 
 float x_cluster_final = 0;
 float y_cluster_final = 0;
 
 if (weight_cluster != 0) {
  x_cluster_final = x_cluster_logE / weight_cluster;
  y_cluster_final = y_cluster_logE / weight_cluster;
  
  _pos_x = x_cluster_final;
  _pos_y = y_cluster_final;
  _energy = energy_cluster;
 }
 
 //  std::cout << " num_clusters = " << num_clusters << std::endl;
}




//---- Reconstruct Calorimeter clusters
void CaloCluster::doCalorimeterReconstruction( std::vector<TBRecHit>* rechits, int face, float maxDR, int fiberLevel) { 
 setRecHits(rechits);
 doCalorimeterReconstruction(face, maxDR, fiberLevel);
} 










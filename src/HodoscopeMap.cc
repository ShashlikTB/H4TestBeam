#include "HodoscopeMap.h"

#include <cmath>



HodoscopeMap::HodoscopeMap(){
 
 std::pair<float, float> temp_position;
 
 int counter = 0;
 for (int iX = 0; iX < 9; iX++) {
  for (int iY = 0; iY < 9; iY++) {
   temp_position.first = -29 + 7.25 * iX;
   temp_position.second = -29 + 7.25 * iY;   
   _map_of_scanning_points[counter] = temp_position;
   counter++;
  }
  
 }
}



HodoscopeMap::~HodoscopeMap(){
}



//---- distance definition
float HodoscopeMap::DR (float x1, float x2, float y1, float y2) {
 return sqrt((x1-x2)*(x1-x2) + (y1-y2)*(y1-y2));
}


int HodoscopeMap::GetCloserIndex(float x, float y){
 
 float min_dr = 10000;
 int min_dr_index = -1;
 for(std::map <int, std::pair<float, float> >::iterator iterator = _map_of_scanning_points.begin(); iterator != _map_of_scanning_points.end(); iterator++) {
  float temp_dr = DR(x, iterator->second.first, y, iterator->second.second);
//   std::cout << " HodoscopeMap::GetCloserIndex::  x = " << x << ", y = " << y << " :: (" << iterator->second.first << "," <<  iterator->second.second << ") :: temp_dr = " << temp_dr;
//   std::cout << "    min_dr = " << min_dr << " <? " << temp_dr << std::endl; 
  if (temp_dr < min_dr) {
   min_dr = temp_dr;
   min_dr_index = iterator->first;
  }
 }

 return min_dr_index;
 
}



std::pair<float, float>  HodoscopeMap::GetCloser(float x, float y){
 
 int index = GetCloserIndex(x,y);
 if (index>=0) {
  return GetValue(index);
 }
 else {
  std::pair<float, float> temp_xy(-999,-999);
  return temp_xy;
 }
}


std::pair<float, float> HodoscopeMap::GetValue(int index){
 return  _map_of_scanning_points[index];
}






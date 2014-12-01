#ifndef HODOSCOPEMAP_H 
#define HODOSCOPEMAP_H

#include <map>
#include <algorithm>
#include <iostream> 

class HodoscopeMap {
 
public:
 
 HodoscopeMap();
 ~HodoscopeMap();
 
 int GetCloserIndex(float x, float y);
 std::pair<float, float> GetValue(int index);
 std::pair<float, float>  GetCloser(float x, float y);
 float DR (float x1, float x2, float y1, float y2);
  
 // private:
 //----    iterator         x      y
 std::map <int, std::pair<float, float> > _map_of_scanning_points; 
};

#endif

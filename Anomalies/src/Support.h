#ifndef _SUPPORT_
#define _SUPPORT_

//includes
#include "Tracklet.h"
#include "RelationGraph.h"
//#include "CUtil.h"

//main definitions 

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
template <class sDataType, class oDataType, class eDataType>
struct Actor {
//definitions...................................................................
  typedef RelationGraph<sDataType, oDataType, eDataType> graphType; 

  //variables.....................................................................
  Tracklet  trk;   //Kalman filter tracklet
 
  graphType graph; //interaction graph 
};

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
/*
inline void loadDefinitions(std::string file, std::map<int,std::string> & info) {
  std::ifstream arc(file);
  for (std::string line; std::getline(arc, line); ) {
    //auto vline = cutil_GetLast    
  }
  arc.close();
}
*/

#endif// _SUPPORT_

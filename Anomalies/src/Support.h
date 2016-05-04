#ifndef _SUPPORT_
#define _SUPPORT_

//includes

#include "CUtil.h"
#include "Tracklet.h"
#include "RelationGraph.h"


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

inline void loadDefinitions(std::string file, std::map<int, std::string> & info) {
  std::ifstream arc(file);
  for (std::string line; std::getline(arc, line); ) {
    auto vline = cutil_string_split(line);
    if (vline.size() > 1) 
      info[std::stoi(vline[0])] = vline[1];
  }
  arc.close();
}


#endif// _SUPPORT_

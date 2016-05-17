#ifndef _SUPPORT_
#define _SUPPORT_

//includes

#include "CUtil.h"
#include "Tracklet.h"
#include "RelationGraph.h"
#include <fstream>
#include <iostream>
#include <functional> 
#include <math.h>


//general namespaces
using namespace std;
using namespace cv;

//main definitions 

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
template <class sDataType, class oDataType, class eDataType>
struct Actor {
//definitions...................................................................
  typedef RelationGraph<sDataType, oDataType, eDataType> graphType; 

  //variables.....................................................................
  TrackingKalman  trk;   //Kalman filter tracklet
 
  graphType       graph; //interaction graph 

  int             old = 0;
  
};
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
struct FrameItem {
  int frameNumber;                  //real frame 
  vector<vector<int> > sub_obj[2];  //0 = subjects 1 = objects
};

///_____________________________________________________________________________
///MAIN SUPPORT CONTROL FUNCTIONS

/////FEAT EXTRACT............................................................... 
//load objects names and ids
void loadDefinitions( string, map<string, int> & );
//load frame items  
void loadFrameItems ( ifstream &, FrameItem &, map<string, int> & );
//load frameList
void loadFrameList  ( string, list<FrameItem> &, short, map<string, int> &);

///_____________________________________________________________________________


#endif// _SUPPORT_

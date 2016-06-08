#ifndef _SUPPORT_
#define _SUPPORT_

//includes

#include "CUtil.h"
#include "Tracklet.h"
#include "RelationGraph.h"
#include "MathUtils.h"
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

struct BaseDefinitions_tr {
  typedef int     sDataType;  // square node data type, representing the frame

  typedef int     oDataType;  // object node data type, representing the index of
                              // general object vector

  typedef double  eDataType;  // edge data type, representing the edge weigth

  typedef RelationGraph<sDataType, oDataType, eDataType> graphType;

};



////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////





template <class tr>
struct Actor_ {

  typedef typename tr::sDataType sDataType;

  typedef typename tr::oDataType oDataType;
                              
  typedef typename tr::eDataType eDataType;

  typedef typename tr::graphType graphType;


//definitions...................................................................
  

  //variables...................................................................
  TrackingKalman  trk_;   //Kalman filter tracklet
 
  graphType       graph_; //interaction graph 

  int             old_        = 0,
                  frame_ini_  = 0;

  bool            visited     = true;

  //main functions..............................................................
  void save2file(string out_file) {
    ofstream arc(out_file, ios::app);
    arc << "G " << frame_ini_ << endl;
    graph_.saving2os(arc);
    arc << endl;
    arc.close();
  }
  //util functions..............................................................
  void runTrk(TrkPoint Center) {
    trk_.newTrack(Center);
    for (int i = 0; i < 20; ++i) {
      trk_.predict();
      trk_.estimate(Center);
    }
  }
  
};

typedef Actor_<BaseDefinitions_tr> Actor;

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
struct FrameItem {
  int frameNumber;                  //real frame 
  vector<vector<int> > sub_obj[2];  //0 = subjects 1 = objects
};

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
struct SubejctLabel {
  int frm_number,
      index_list_frm;
};
struct SubejctLabel {
  int frm_number,
      index_list_frm;
};

///_____________________________________________________________________________
///MAIN SUPPORT CONTROL FUNCTIONS

/////FEAT EXTRACT............................................................... 
////GRAPH BUILD.................................................................
//load objects names and ids
void loadDefinitions( string, map<string, int> & );

//load frame items  
void loadFrameItems ( ifstream &, FrameItem &, map<string, int> & );
//load frameList

void loadFrameList  ( string, list<FrameItem> &, short, map<string, int> &);
//compute the distance point to object bounding box
double  distance2object(  TrkPoint /*point*/, 
                          TrkPoint /*NortWest point*/, 
                          TrkPoint /*SouthEast point*/);

///_____________________________________________________________________________
////GRAPH COMPARISON 
//load described graph
void loadDescribedGraphs(string, list<BaseDefinitions_tr::graphType> &);

#endif// _SUPPORT_

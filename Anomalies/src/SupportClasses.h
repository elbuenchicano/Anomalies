#ifndef SUPPORTCLASSES_H
#define SUPPORTCLASSES_H

//##############################################################################
//##############################################################################
//##############################################################################
//MAIN INCLUDES FOR ENGINE SOLUTION

#include "opencv2/core/core.hpp"
#include "opencv2/imgcodecs.hpp"
#include "opencv2/highgui/highgui.hpp"

#include "CUtil.h"
#include "Tracklet.h"
#include "RelationGraph.h"
#include "MathUtils.h"
#include "fmSequence.h"
#include "AlgoUtil.h"
#include "Sequence.h"

#include <fstream>
#include <iostream>
#include <functional> 
#include <math.h>
#include <algorithm>
#include <set>

//##############################################################################
//##############################################################################
//general namespaces
using namespace std;
using namespace cv;

//main definitions 

//##############################################################################
//##############################################################################
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
struct ActorLabel;

struct BaseDefinitions_tr {
  typedef ActorLabel  sDataType;  // square node data type, 

  typedef ActorLabel  oDataType;  // object node data type, 

  typedef double      eDataType;  // edge datatype, representing the edge weigth

  typedef RelationGraph<sDataType, oDataType, eDataType> graphType;

};

//##############################################################################
//##############################################################################
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//This class must be in the same .h file because the template
//it represent the first structure to join the tracklets and build the first 
//graph representation
template <class tr>
struct Actor_ {
  //definitions...................................................................
  typedef typename tr::sDataType sDataType;

  typedef typename tr::oDataType oDataType;

  typedef typename tr::eDataType eDataType;

  typedef typename tr::graphType graphType;

  //variables...................................................................
  TrackingKalman  trk_;   //Kalman filter tracklet

  graphType       graph_; //interaction graph 

  int             old_ = 0,
                  id_  = 0;

  bool            visited = true,
                  picked  = false;

  //main functions..............................................................
  void save2file(ostream &os) {
    os << "G " << id_ << endl;
    graph_.saving2os(os);
    os << endl;
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
//template trait definition of actor
typedef Actor_<BaseDefinitions_tr> Actor;




////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

struct FrameItem {
  int frameNumber;                  //real frame 
  vector<vector<int> > sub_obj[2];  //0 = subjects 1 = objects
};

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
struct ActorLabel {
  int id_,
      list_idx_;
  ActorLabel(){}
  ActorLabel(int, int);
  //funtions
  ActorLabel &operator=(ActorLabel &);
};
//actor label overload function
ostream &operator <<(ostream & os, ActorLabel & ac);



#endif//SUPPORTCLASSES_H
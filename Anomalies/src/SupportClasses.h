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

  Point           sw_,
                  ne_;

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
struct ActorLabel {
  int   id_,
        end_,
        list_idx_;

  bool  anomaly_ = false;

  ActorLabel() {}
  ActorLabel(int, int);
  //funtions
  ActorLabel &operator=(ActorLabel &);
};
//actor label overload function
ostream &operator <<(ostream & os, ActorLabel & ac);

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
struct Observed {

  int id_;
  
  typedef BaseDefinitions_tr::graphType graphType;

  graphType     graph_;

  vector<bool>  levels_;

  Observed(int id) :id_(id) { levels_.resize(5); };
  ~Observed() {};
};

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

//Anomaly structure
struct AnomalyGt {
  int     id_   = 0,
          ini_  = 0,
          fin_  = 0,
          type_ = 0;
  string  desc_ ="";
  //subject id
  //initial frame
  //final frame
  //anomaly type
  //anomaly description
  AnomalyGt(int, int, int, int, string &);
  AnomalyGt() {};
  AnomalyGt operator =  (const AnomalyGt &);
};
bool operator == (const AnomalyGt &, const AnomalyGt &);
bool operator <  (const AnomalyGt &, const AnomalyGt &);

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
struct Palete : public Scalar {
  enum colors{RED,BLUE,GREEN};
};

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
///specific anomalie control definitions

typedef BaseDefinitions_tr::graphType graphType;
typedef list<Observed>                graphLstT;
typedef bool(*pf) ( string &, short, map<string, int> *, map<int, string> *,
                    list<Observed>  &, set<int>&, int);

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///Olds classes
struct FrameItem {
  int frameNumber;                  //real frame 
  vector<vector<int> > sub_obj[2];  //0 = subjects 1 = objects
};



#endif//SUPPORTCLASSES_H
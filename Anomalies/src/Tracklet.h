#ifndef _TRACKLET_
#define _TRACKLET_

//.............................................................................
//general includes
#include "Tracking.h"
#include <opencv2/highgui/highgui.hpp>
#include <memory>

//.............................................................................
//main definitions
typedef cv::Rect2f TrlRegion; //same as kalman tracking

///tracklet class for object tracking and image location
struct Tracklet {

  int       frm_ini_,
            frm_fin_;
  
  TrlRegion  region_;

  TrackingBase  *trk_ = nullptr;

  //contructors and destructors
  Tracklet(TrlRegion &);
  ~Tracklet();

  //main functions
  void find_next(TrlRegion &);
};

#endif //_TRACKLET_

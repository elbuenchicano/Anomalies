#ifndef TRACKLET_H
#define TRACKLET_H

//.............................................................................
//general includes
#include "Tracking.h"
#include <opencv2/highgui/highgui.hpp>
#include <memory>

//.............................................................................
//main definitions
typedef cv::Point2f TrlRegion; //same as kalman tracking

///tracklet class for object tracking and image location
class Tracklet {

  
  TrackingKalman trk_;

public:
  int       frm_ini_,
            frm_fin_;

  TrlRegion region_;


  //contructors and destructors
  Tracklet();
  ~Tracklet();

  //main functions
  void find_next(TrlRegion &);
  void initiate(TrlRegion &);
};

#endif //_TRACKLET_

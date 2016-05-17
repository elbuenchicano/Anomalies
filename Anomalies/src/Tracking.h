#ifndef TRACKING_H
#define TRACKING_H

//include libs.................................................................

#include "opencv2/core/core.hpp"
#include "opencv2/imgcodecs.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/video/tracking.hpp"


//global definitions...........................................................

// kalman mouse example 
#define TRK_KALMAN_EXAMPLE_ 
typedef cv::Point2f   TrkPoint;

///main class for tracking
struct TrackingBase{
  TrackingBase()  {}
  ~TrackingBase() {};

  //main functions
  virtual void      newTrack(TrkPoint &) = 0;
  virtual TrkPoint  predict() = 0;
  virtual TrkPoint  estimate(TrkPoint &) = 0;
};

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
///tracking class using opencv kalman filter
struct TrackingKalman : public TrackingBase {

  cv::KalmanFilter  KF_;

public:

  TrackingKalman();
  ~TrackingKalman();
  virtual void      newTrack(TrkPoint &);
  virtual TrkPoint  predict();
  virtual TrkPoint  estimate(TrkPoint &);

};

#ifdef  TRK_KALMAN_EXAMPLE_
  void    trk_kalmanMouseExample();
#endif 

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

#endif //_TRACKING_


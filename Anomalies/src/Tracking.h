#ifndef _TRACKING_
#define _TRACKING_

//include libs.................................................................

#include <opencv2/core/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/video/tracking.hpp>

//global definitions...........................................................

typedef cv::Point2f   TrkPoint;

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

struct TrackingKalman : public TrackingBase {

  cv::KalmanFilter KF;

public:

  TrackingKalman();
  ~TrackingKalman();
  virtual void      newTrack(TrkPoint &);
  virtual TrkPoint  predict();
  virtual TrkPoint  estimate(TrkPoint &);

};

#endif //_TRACKING_


#include "Tracking.h"

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//.................Tracking Kalman.............................................
//constructors and destructors 

TrackingKalman::TrackingKalman() {}
TrackingKalman::~TrackingKalman() {}

////////////////////////////////////////////////////////////////////////////////
//virtual functions 

///initiate new observation 
///in: point
void TrackingKalman::newTrack(TrkPoint &centroid)
{
  //6 dimensions(Velocity (Vx,Vy), Position (Px,Py) e Aceleration (Ax,Ay))
  KF = cv::KalmanFilter(6, 2, 0);
  cv::Mat_<float> state(6, 1);
  cv::Mat processNoise(6, 1, CV_32F);

  //Initiate the statePre and Post with position given by HOG detection.
  KF.statePre.at<float>(0) = (float)centroid.x;
  KF.statePre.at<float>(1) = (float)centroid.y;

  KF.statePre.at<float>(2) = 0;
  KF.statePre.at<float>(3) = 0;
  KF.statePre.at<float>(4) = 0;
  KF.statePre.at<float>(5) = 0;

  KF.statePost.at<float>(0) = (float)centroid.x;
  KF.statePost.at<float>(1) = (float)centroid.y;

  KF.statePost.at<float>(2) = 0;
  KF.statePost.at<float>(3) = 0;
  KF.statePost.at<float>(4) = 0;
  KF.statePost.at<float>(5) = 0;

  KF.transitionMatrix  = (cv::Mat_<float>(6, 6) << 1, 0, 1, 0, 0.5, 0, 0, 1, 0, 1, 0, 0.5, 0, 0, 1, 0, 1, 0, 0, 0, 0, 1, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1);

  KF.measurementMatrix = (cv::Mat_<float>(2, 6) << 1, 0, 1, 0, 0.5, 0, 0, 1, 0, 1, 0, 0.5);

  setIdentity(KF.processNoiseCov, cv::Scalar::all(.005)); //adjust this for faster convergence - but higher noise
  setIdentity(KF.measurementNoiseCov, cv::Scalar::all(4));
  setIdentity(KF.errorCovPost, cv::Scalar::all(.1));//.1

  predict();
  estimate(centroid);

}
///predict the next position if you dont know where may be
///out: point 
TrkPoint TrackingKalman::predict() {
  TrkPoint p;

  cv::Mat prediction = KF.predict();
  
  p.x = prediction.at<float>(0);
  p.y = prediction.at<float>(1);

  return p;
}
///estimate the new position given a new observation
///in: obs point
///out: point
TrkPoint TrackingKalman::estimate(TrkPoint & position) {
  
  

  TrkPoint e;
  cv::Mat measurement = cv::Mat::zeros(2, 1, CV_32F);

  measurement.at<float>(0) = (float)position.x;
  measurement.at<float>(1) = (float)position.y;

  cv::Mat estimated = KF.correct(measurement);
 
  e.x = estimated.at<float>(0);
  e.y = estimated.at<float>(1);

  return predict();
}

//.............................................................................
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////


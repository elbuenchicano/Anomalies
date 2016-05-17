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
  KF_ = cv::KalmanFilter(6, 2, 0);
  cv::Mat_<float> state(6, 1);
  cv::Mat processNoise(6, 1, CV_32F);

  //Initiate the statePre and Post with position given by HOG detection.
  KF_.statePre.at<float>(0) = (float)centroid.x;
  KF_.statePre.at<float>(1) = (float)centroid.y;

  KF_.statePre.at<float>(2) = 0;
  KF_.statePre.at<float>(3) = 0;
  KF_.statePre.at<float>(4) = 0;
  KF_.statePre.at<float>(5) = 0;

  KF_.transitionMatrix  = (cv::Mat_<float>(6, 6) << 1, 0, 1, 0, 0.5, 0, 0, 1, 0, 1, 0, 0.5, 0, 0, 1, 0, 1, 0, 0, 0, 0, 1, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1);

  KF_.measurementMatrix = (cv::Mat_<float>(2, 6) << 1, 0, 1, 0, 0.5, 0, 0, 1, 0, 1, 0, 0.5);

  setIdentity(KF_.processNoiseCov, cv::Scalar::all(1e-2)); //adjust this for faster convergence - but higher noise
  setIdentity(KF_.measurementNoiseCov, cv::Scalar::all(2));
  setIdentity(KF_.errorCovPost, cv::Scalar::all(.5));//.1

  //predict();
  //estimate(centroid);

}
///predict the next position if you dont know where may be
///out: point 
TrkPoint TrackingKalman::predict() {
  TrkPoint p;

  cv::Mat prediction = KF_.predict();
  
  p.x = prediction.at<float>(0);
  p.y = prediction.at<float>(1);

  return p;
}
///estimate the new position given a new observation
///in: obs point
///out: point
TrkPoint TrackingKalman::estimate(TrkPoint & position) {
  
  //predict();
  
  TrkPoint e;
  cv::Mat measurement = cv::Mat::zeros(2, 1, CV_32F);

  measurement.at<float>(0) = (float)position.x;
  measurement.at<float>(1) = (float)position.y;

  cv::Mat estimated = KF_.correct(measurement);
 
  e.x = estimated.at<float>(0);
  e.y = estimated.at<float>(1);

  return e;
}

//.............................................................................
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//EXAMPLES 
//KALMAN TRACKING

#ifdef  TRK_KALMAN_EXAMPLE_
#include <Windows.h>

#define drawCross( center, color, d )                                 \
line( img, TrkPoint( center.x - d, center.y - d ), TrkPoint( center.x + d, center.y + d ), color, 2, CV_AA, 0); \
line( img, TrkPoint( center.x + d, center.y - d ), TrkPoint( center.x - d, center.y + d ), color, 2, CV_AA, 0 )

using namespace cv;
using namespace std;

void trk_kalmanMouseExample()
{
  TrackingKalman trk;
  trk.newTrack(TrkPoint(100, 100));

  POINT mousePos;
  GetCursorPos(&mousePos);

  Mat img(600, 800, CV_8UC3);
  vector<Point> mousev, kalmanv;
  mousev.clear();
  kalmanv.clear();

  while (1)
  {
    Point predictPt = trk.predict();

    GetCursorPos(&mousePos);
    TrkPoint measPt(static_cast<float>( mousePos.x ), 
                    static_cast<float>( mousePos.y ));

    TrkPoint statePt = trk.estimate(measPt);

    imshow("mouse kalman", img);
    img = Scalar::all(0);

    mousev.push_back(measPt);
    kalmanv.push_back(statePt);
    drawCross(statePt, Scalar(255, 255, 255), 5);
    drawCross(measPt, Scalar(0, 0, 255), 5);

    for (int i = 0; i < mousev.size() - 1; i++)
      line(img, mousev[i], mousev[i + 1], Scalar(255, 255, 0), 1);

    for (int i = 0; i < kalmanv.size() - 1; i++)
      line(img, kalmanv[i], kalmanv[i + 1], Scalar(0, 155, 255), 1);

    waitKey(10);
  }
}
#endif //KALMAN_EXAMPLE_


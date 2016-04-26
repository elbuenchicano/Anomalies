#include "track.h"

using namespace ssf;

track::track(void)
{
	age = 0;
	detectedConsecutive = 0;
	numFramesInvisible =0;
}

track::~track(){

}

void track::InitializeKalman(SSFPoint centroid) {

	//6 dimensions(Velocity (Vx,Vy), Position (Px,Py) e Aceleration (Ax,Ay))
	KF = cv::KalmanFilter(6, 2, 0);
	cv::Mat_<float> state(6,1);
	cv::Mat processNoise(6, 1, CV_32F);

	//Initiate the statePre and Post with position given by HOG detection.
	KF.statePre.at<float>(0) = (float) centroid.x;
	KF.statePre.at<float>(1) = (float) centroid.y;
	
	KF.statePre.at<float>(2) = 0;
	KF.statePre.at<float>(3) = 0;
	KF.statePre.at<float>(4) = 0;
	KF.statePre.at<float>(5) = 0;

	KF.statePost.at<float>(0) = (float) centroid.x;
	KF.statePost.at<float>(1) = (float) centroid.y;

	KF.statePost.at<float>(2) = 0;
	KF.statePost.at<float>(3) = 0;
	KF.statePost.at<float>(4) = 0;
	KF.statePost.at<float>(5) = 0;

	KF.transitionMatrix = *(cv::Mat_<float>(6, 6) << 1,0,1,0,0.5,0, 0,1,0,1,0,0.5, 0,0,1,0,1,0, 0,0,0,1,0,1, 0,0,0,0,1,0, 0,0,0,0,0,1);

	KF.measurementMatrix = *(cv::Mat_<float>(2, 6) << 1,0,1,0,0.5,0, 0,1,0,1,0,0.5);

	setIdentity(KF.processNoiseCov, cv::Scalar::all(.001)); //adjust this for faster convergence - but higher noise
	setIdentity(KF.measurementNoiseCov, cv::Scalar::all(1e-1)); 
	setIdentity(KF.errorCovPost, cv::Scalar::all(.1));//.1

}

DetWin track::PredictPositions() {
	DetWin p;

	cv::Mat prediction = KF.predict();

	p.x0 = (int) prediction.at<float>(0);
	p.y0 = (int) prediction.at<float>(1);

	return p;
}

DetWin track::EstimatedPositions(SSFPoint position) {

	DetWin e;
	cv::Mat measurement = cv::Mat::zeros(2, 1, CV_32F);

	measurement.at<float>(0) = (float) position.x;
	measurement.at<float>(1) = (float) position.y;

	cv::Mat estimated = KF.correct(measurement);
	e.x0 = (int) estimated.at<float>(0);
	e.y0 = (int) estimated.at<float>(1);

	return e;
}

SSFUserDataPedestrian::SSFUserDataPedestrian(): SMUserData(){
}

//
//samplePed::samplePed(void){
//}
//
//samplePed::~samplePed(){
//}

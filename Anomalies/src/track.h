#ifndef _TRACK_H_ 
#define _TRACK_H_

#include "LAP.h" 
#include "ModKalmanTracking.h"
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/video/tracking.hpp>
#include <stdio.h>
#include <UserModule.h>
#include "SMTracklet.h"

namespace ssf {
	class SSFUserDataPedestrian: public SMUserData {
		friend class ModKalmanTracking;
		friend class ModTrafficLight;

	public:
		SMIndexType frameID;		// frame ID
		int x, y, w, h;
		float vx, vy;

		SSFUserDataPedestrian();
		//~SSFUserDataPedestrian();
	};

	class track{
		friend class ModKalmanTracking;
		friend class SSFUserDataPedestrian;

		cv::KalmanFilter KF;
		std::vector<SMSample *>tracklet;
		SMIndexType objID;
		SMIndexType flag;
		int age; //used to decrease the false positive number.
		int detectedConsecutive; // number of frames consecutively detected
		int numFramesInvisible; // number of frames since that the subject disappears
		DetWin detection;  //HOG Detection
		DetWin prediction;     //Kalman Prediction 
		DetWin estimated; //Kalman Estimation 
		DetWin PredictPositions();
		DetWin EstimatedPositions(SSFPoint position);

	public:

		track(void);
		~track();

		void InitializeKalman(SSFPoint centroid);

	};
}

#endif
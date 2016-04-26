#ifndef _KALMANFILTER_H_ //Evitar a chamada recursiva do .h
#define _KALMANFILTER_H_

#include "SMTracklet.h"
#include "track.h"
#include <UserModule.h>

namespace ssf{
	class track;

	class ModKalmanTracking : public UserModule {

		std::string inModSamples;
		std::string inModObj;

		int dynamParams;
		int measureParams;
		int numFramesInv;
		int minSizeTracklet;

		std::unordered_map<std::string,track> models;

		void PredictAll(std::vector<SMIndexType> detections);
		void NewTrack(SMSample *detect, SMIndexType samplesID, SMObject *obj);
		void UpdateModelswithDetect(SMSample *detect, SMIndexType samplesID, std::string objID);
		void UpdateModelswithoutDetect(SMIndexType samplesID, std::string objID);
		void PredictMissingTracks(int i);

	public:
		ModKalmanTracking(std::string modID);//Construtor 
		~ModKalmanTracking();

		// return ID of the input format    
		std::string GetName() { return "ModKalmanTracking"; }

		// Return the type of the module  
		std::string GetClass() { return SSF_USERMOD_CLASS_Mod_Tracking; }

		// retrieve structure version
		std::string GetVersion() { return "0.0.1"; }

		// function to generate a new instatiation of the detector
		Method *Instantiate(std::string modID);

		// function to check if the parameters are set correctly
		void Setup();

		// execute
		void Execute();

	};
}
#endif

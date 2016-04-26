//#include "headers.h"
#include "ssf_defs.h"
#include "LAP.h" //Looking at people
#include "ModKalmanTracking.h"
#include "SMData.h"
#include "Misc.h" 
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/video/tracking.hpp>
#include <stdio.h>

using namespace ssf;

class track;

ModKalmanTracking instance("");

/******
* class ModKalmanTracking
******/
ModKalmanTracking::ModKalmanTracking(std::string modID) :  UserModule(modID) {

	/* set default values */
	dynamParams = 4;
	measureParams = 2;
	numFramesInv = 0;
	minSizeTracklet = 0;

	/* Add parameters in file paramstest*/
	inputParams.add("dynamParams", &dynamParams, "Dimensionality of the state. Default: 4", false); 
	inputParams.add("measureParams", &measureParams, "Dimensionality of the measurement. Default: 2", false); 
	inputParams.add("numFramesInvisible", &numFramesInv, "Number of frames that the pedestrian can be disappeared", false);
	inputParams.add("minSizeTracklet", &minSizeTracklet, "Minimum size that a tacklet must have to be put in the SM", false);



	/* set input and output data */
	modParams.Require(modID,  SMTYPE_SAMPLES, &inModSamples);	// input samples
	modParams.Require(modID,  SMTYPE_OBJECT, &inModObj); //input map
	modParams.Provide(modID, SMTYPE_TRACKLET); // output mask
	modParams.Provide(modID, SMTYPE_SAMPLES); // output samples
	modParams.Provide(modID, SMTYPE_USERDATA); //output user data
	modParams.Provide(modID, SMTYPE_MAP); //output Map

	/* module information */
	info.setDescription("Module to tracking pedestrians");

	/* register the module */
	this->Register(this, modID);
}


Method *ModKalmanTracking::Instantiate(std::string modID) {

	return new ModKalmanTracking(modID); 
}

ModKalmanTracking::~ModKalmanTracking(){
}

void ModKalmanTracking::Setup() {

	if (inModSamples == "")
		ReportError("Input module with samples has not been set (variable: inModSamples)!");
	if (inModObj == "")
		ReportError("Input module with objects has not been set (variable: inModObj)!");
}


void ModKalmanTracking::NewTrack(SMSample *detect, SMIndexType samplesID, SMObject *obj){
	track model;
	Rect rectSample;
	SSFPoint centroid;
	size_t sampleID;

	rectSample = detect->getWindow();

	centroid.x=rectSample.x0 + (rectSample.w/2);
	centroid.y=rectSample.y0 + (rectSample.h/2);

	model.age=1;
	model.detectedConsecutive = 1;
	model.detection.x0 = rectSample.x0;
	model.detection.y0 = rectSample.y0;
	model.detection.w = rectSample.w;
	model.detection.h = rectSample.h;

	//Initialize the Kalman Filter
	model.InitializeKalman(centroid);

	//Corrige a bounding da predição, que foi feita no centroide.
	model.prediction = model.PredictPositions();
	model.prediction.w = rectSample.w;
	model.prediction.h = rectSample.h;

	model.estimated = model.EstimatedPositions(centroid);
	model.estimated.w = rectSample.w;
	model.estimated.h = rectSample.h;

	//Set samples
	SSFUserDataPedestrian *sPed= new SSFUserDataPedestrian();
	sampleID = glb_sharedMem.createSample(samplesID,detect->getWindow(),detect->getResponse());

	sPed->x = rectSample.x0;
	sPed->y = rectSample.y0;
	sPed->w = rectSample.w;
	sPed->h = rectSample.h;
	sPed->frameID = detect->getFrameID();
	sPed->vx = model.KF.statePre.at<float>(2);
	sPed->vy = model.KF.statePre.at<float>(3);
	model.tracklet.push_back(detect);

	//SMSample *smp = glb_sharedMem.getSample(sampleID);
	////send the tracklets for the shared memory
	//model.tracklet = glb_sharedMem.getTracklet(glb_sharedMem.createTracklet(""));
	//model.tracklet->addSample(smp);

	//glb_sharedMem.unlinkDataItem(smp);
	//send the UserData for the shared memory
	glb_sharedMem.addUserData((SMUserData**)&sPed,"");

	model.objID = obj->getSMID();

	models.insert(std::pair<std::string,track>(obj->getObjID(),model));
}

void ModKalmanTracking::UpdateModelswithDetect(SMSample *detect, SMIndexType samplesID, std::string objID){

	Rect rectSample;
	SSFPoint centroid;
	SMIndexType sampleID;

	rectSample = detect->getWindow();

	centroid.x=rectSample.x0 + (rectSample.w/2);
	centroid.y=rectSample.y0 + (rectSample.h/2);

	models[objID].age++;
	models[objID].detectedConsecutive++;
	models[objID].numFramesInvisible = 0;
	models[objID].detection.x0 = rectSample.x0;
	models[objID].detection.y0 = rectSample.y0;
	models[objID].detection.w = rectSample.w;
	models[objID].detection.h = rectSample.h;

	//Corrige a bounding da predição, que foi feita no centroide.
	models[objID].prediction = models[objID].PredictPositions();
	models[objID].prediction.w = rectSample.w;
	models[objID].prediction.h = rectSample.h;

	models[objID].estimated = models[objID].EstimatedPositions(centroid);
	models[objID].estimated.w = rectSample.w;
	models[objID].estimated.h = rectSample.h;

	//Set samples
	SSFUserDataPedestrian *sPed= new SSFUserDataPedestrian();
	sampleID = glb_sharedMem.createSample(samplesID,detect->getWindow(),detect->getResponse());

	sPed->x = rectSample.x0;
	sPed->y = rectSample.y0;
	sPed->w = rectSample.w;
	sPed->h = rectSample.h;
	sPed->frameID = detect->getFrameID();
	sPed->vx = models[objID].KF.statePre.at<float>(2);
	sPed->vy = models[objID].KF.statePre.at<float>(3);

	models[objID].tracklet.push_back(detect);

	//SMSample *smp = glb_sharedMem.getSample(sampleID);
	////send the tracklets for the shared memory
	//models[objID].tracklet->addSample(smp);

	//glb_sharedMem.unlinkDataItem(smp);
	//send the UserData for the shared memory
	glb_sharedMem.addUserData((SMUserData**) &sPed,"");

}
void ModKalmanTracking::UpdateModelswithoutDetect(SMIndexType samplesID, std::string objID){
	SMIndexType sampleID;

	models[objID].prediction = models[objID].PredictPositions();
	models[objID].prediction.w = models[objID].detection.w;
	models[objID].prediction.h = models[objID].detection.h;
	models[objID].numFramesInvisible++;
	models[objID].detectedConsecutive=0;
	models[objID].age++;	

	//Set samples
	Rect window = models[objID].prediction.getSSFRect();
	window.x0 = window.x0 - (models[objID].detection.w/2);
	window.y0 = window.y0 - (models[objID].detection.h/2);
	window.w = models[objID].detection.w;
	window.h = models[objID].detection.h;

	float soma=0, media;
	for(SMSample *s : models[objID].tracklet){
		soma += s->getResponse();
	}
	media = soma/(float)models[objID].tracklet.size();

	SSFUserDataPedestrian *sPed= new SSFUserDataPedestrian();
	sampleID = glb_sharedMem.createSample(samplesID,window,media);

	SMSample *smp = glb_sharedMem.getSample(sampleID);
	sPed->x = window.x0;
	sPed->y = window.y0;
	sPed->w = window.w;
	sPed->h = window.h;
	sPed->frameID = smp->getFrameID();
	sPed->vx = models[objID].KF.statePre.at<float>(2);
	sPed->vy = models[objID].KF.statePre.at<float>(3);

	models[objID].tracklet.push_back(smp);

	//send the tracklets for the shared memory
	//models[objID].tracklet->addSample(smp);
	//send the UserData for the shared memory
	glb_sharedMem.addUserData((SMUserData**) &sPed,"");
	//glb_sharedMem.unlinkDataItem(smp);
}

void ModKalmanTracking::Execute() {
	SMIndexType smKalmanID, pulseID, streamID, frameID;	
	size_t position = 0, posPulse = 0;

	long double bestSim= DBL_MAX;
	long double diff=0, incr=0;
	cv::Mat data;
	SMSamples *SMsamples, *smKalman;
	std::vector<SMSample *> vecSamples, samplesObj;

	int m=0;

	SMObject *obj;
	SMSample *sampleObj;
	int unknown=1;

	while(SMsamples = glb_sharedMem.getSamplesbyPosition(inModSamples, position++)){
		vecSamples = SMsamples->getSamples();

		smKalmanID = glb_sharedMem.createSetofSamples(SMsamples->getFrameID(),"");
		smKalman = glb_sharedMem.getSetSamples(smKalmanID);

		if(vecSamples.size() <= 0) continue;

		//for each sample detected by HOG in this current frame been processed
		for(SMSample *sample : vecSamples){
			//map with sample and distance of the compare between sample and model
			std::map<SMIndexType, float> *map = new std::map<SMIndexType, float>();
			//for each model already existent
			for(const std::pair<std::string,track> &mod : models){

				if(mod.second.numFramesInvisible <= numFramesInv){
					//get the window
					Rect rectSample = sample->getWindow();
					SSFPoint centroid;
					centroid.x= (int) rectSample.x0 + (rectSample.w/2);
					centroid.y= (int) rectSample.y0 + (rectSample.h/2);
					//compute the euclidian distance between sample and model
					map->insert( std::pair<SMIndexType,float> (mod.second.objID,(float)(sqrt((pow(centroid.x - mod.second.prediction.x0,2)) + pow((centroid.y - mod.second.prediction.y0),2)))));
				}
			}

			//send the map to shared memory
			glb_sharedMem.createMap<SMIndexType,float>(sample->getSMID(),&map);

			glb_sharedMem.unlinkDataItem(sample);
		}
		pulseID = glb_sharedMem.sendSyncPulse(SMTYPE_MAP,"");

		// get ID of the input stream from which the pulse will be written
		streamID = glb_sharedMem.getDataStreamID(inModObj, SMTYPE_OBJECT);
		glb_sharedMem.waitForSyncPulse(streamID,pulseID);

		//Get object 
		while(!glb_sharedMem.checkForSyncPulse(streamID,posPulse,pulseID)){
			unknown = 1;
			obj = glb_sharedMem.getObjectbyPosition(inModObj,posPulse++);
			//get the samples of the objetc
			samplesObj = obj->getSamples();
			//the object must have only one sample
			if(samplesObj.size() != 1)
				ReportError("Object contains more than one sample");
			else
				sampleObj = samplesObj.at(0);

			if (models.empty())
				NewTrack(sampleObj,smKalmanID,obj);

			else{
				//for each model already existent
				for(std::pair<const std::string,track> &mod : models){

					if(mod.second.numFramesInvisible <= numFramesInv){
						//if object already exist in the model
						if(mod.first.compare(obj->getObjID()) == 0){
							//update the model
							UpdateModelswithDetect(sampleObj,smKalmanID,obj->getObjID());
							mod.second.flag = smKalman->getFrameID();
							unknown=0;
							break;
						}
					}
				}
				if(unknown)
					NewTrack(sampleObj,smKalmanID,obj);

			}
			//glb_sharedMem.unlinkDataItem(samplesObj.at(0));
			glb_sharedMem.unlinkDataItem(obj);
		}
		//update the models that have not been updated
		for(const std::pair<std::string,track> &mod : models){
			if(mod.second.numFramesInvisible <= numFramesInv){
				if(mod.second.flag != smKalman->getFrameID()){
					UpdateModelswithoutDetect(smKalmanID, mod.first);
				}
			}
		}
		ReportStatus("Kalman for frameID '%d'", position);

		smKalman->close();
		SMsamples->close();
		glb_sharedMem.unlinkDataItem(smKalman);
		glb_sharedMem.unlinkDataItem(SMsamples);
	}

	for(std::pair<const std::string,track> &mod : models){
		if(mod.second.tracklet.size() > minSizeTracklet){
			SMTracklet * tracklet = glb_sharedMem.getTracklet(glb_sharedMem.createTracklet(""));
			for(SMSample *sample : mod.second.tracklet){
				tracklet->addSample(sample);
				glb_sharedMem.unlinkDataItem(sample);
			}
			tracklet->close();
			glb_sharedMem.unlinkDataItem(tracklet);
		}
		else{
			for(SMSample *sample : mod.second.tracklet)
				glb_sharedMem.unlinkDataItem(sample);
		}
	}
}



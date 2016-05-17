#include "AnomalieControl.h"

///default constructor
AnomalieControl::AnomalieControl(){}
///constructor string
AnomalieControl::AnomalieControl(string file):
main_prop_file_(file){
  
  string main_object_file;

  //reading the main file settings

  fs_main_.open(file, FileStorage::READ);
  assert(fs_main_.isOpened());
  fs_main_["main_object_file"]  >> main_object_file;
  fs_main_["main_frame_step"]  >> frame_step_;
  
  //loading functions
  loadDefinitions(main_object_file, objects_);

}
///default destructor
AnomalieControl::~AnomalieControl(){}
///
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
void AnomalieControl::run() {
  short op;
  fs_main_["main_operation"] >> op;
  switch (op)
  {
  case 1:
    featExtract();
    break;
  default:
    break;
  }
}


////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
void AnomalieControl::featExtract() {
  typedef int sDataType;  // square node data type, representing the frame

  typedef int oDataType;  // object node data type, representing the index of
                          // general object vector

  typedef double eDataType;  // edge data type, representing the edge weigth

  typedef Actor<sDataType, oDataType, eDataType> actorType;
  
  //variables..................................................................
  list< actorType > active_sub;

  double            distance_obj_thr,
                    distance_sub_thr;

  int               time_life;

  //............................................................................
  string          seq_file;
  list<FrameItem> frame_list;
  fs_main_["featExtract_seq_file"]          >> seq_file;
  fs_main_["featExtract_distance_sub_thr"]  >> distance_sub_thr;
  fs_main_["featExtract_distance_obj_thr"]  >> distance_obj_thr;
  fs_main_["featExtract_time_life"]         >> time_life;

  //............................................................................
  loadFrameList(seq_file, frame_list, frame_step_, objects_);
  
  //............................................................................
  //Tracking and looking for subjects
  //for each frame in parsed video
  for (auto & frame : frame_list) {

    //for each candidate subject update the list 
    for (auto & subj : frame.sub_obj[0]) {
     
      double  mindist = FLT_MAX;
      auto    nearest = active_sub.end();
      Point2f subCenter( static_cast<float>( (subj[1] + subj[3]) / 2.0), 
                         static_cast<float>( (subj[2] + subj[4]) / 2.0) );
      
      //for each active subject
      for (auto ite = active_sub.begin(); ite != active_sub.end(); ite++) {
        

        auto dist = cv::norm( ite->trk.region_ - subCenter );
        if (dist < distance_sub_thr) {
          mindist = dist;
          nearest = ite;
        }
      }

      if (nearest == active_sub.end()) {
        actorType new_active;
        new_active.trk.initiate(subCenter);
        new_active.trk.frm_ini_ = frame.frameNumber;
        new_active.graph.addSubjectNode(frame.frameNumber);
        active_sub.push_back(new_active);
        nearest--;
      }
      else {
        nearest->graph.addSubjectNode(frame.frameNumber);
        nearest->trk.find_next(subCenter);
      }
      
      //!!!!!!!!!!!!!!!!!!!!!!!
      //MAY CODING WITH THREADS
      //for each candidate update the object interaction
      for (auto & obj : frame.sub_obj[1]) {

        //!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
        //HUMAN POSE ESTIMATION
        //INITIALY WITH EUCLIDEAN DISTANCE
        Point2f objCenter(
          static_cast<float> ((obj[1] + obj[3]) / 2.0), 
          static_cast<float> ((obj[2] + obj[4]) / 2.0) );
        auto dist = cv::norm(nearest->trk.region_ - objCenter);
        if (dist < distance_obj_thr) {
          nearest->graph.addObjectRelation(obj[0], dist);
        }
      }
    }
  }

  
}


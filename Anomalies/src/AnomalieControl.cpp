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
  fs_main_["main_frame_step"]   >> frame_step_;
  
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
    graphBuilding();
    break;
  default:
    break;
  }
}


////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
void AnomalieControl::graphBuilding() {
 
  
  //variables..................................................................
  list< Actor >     active_sub;

  double            distance_obj_thr,
                    distance_sub_thr;

  int               time_life;

  string            out_file,
                    seq_file;

  list<FrameItem>   frame_list;
  //............................................................................
   
  fs_main_["featExtract_seq_file"]          >> seq_file;
  fs_main_["featExtract_distance_sub_thr"]  >> distance_sub_thr;
  fs_main_["featExtract_distance_obj_thr"]  >> distance_obj_thr;
  fs_main_["featExtract_time_life"]         >> time_life;
  fs_main_["featExtract_out_file"]          >> out_file;

  //............................................................................
  loadFrameList(seq_file, frame_list, frame_step_, objects_);

  //............................................................................
  //Tracking and looking for subjects
  //for each frame in parsed video
  for (auto & frame : frame_list) {
    cout << frame.frameNumber<< endl;
    //for each candidate subject update the list 
    for (auto & subj : frame.sub_obj[0]) {
     
      double    mindist = FLT_MAX;
      auto      nearest = active_sub.end();

      TrkPoint  subCenter( static_cast<float>( (subj[1] + subj[3]) / 2.0), 
                           static_cast<float>( (subj[2] + subj[4]) / 2.0) );

      //////////////////////////////////////////////////////////////////////
      //forget this part when bug is fixed
      
      /*TrkPoint  hand1    ( static_cast<float>( subj[6]),  
                           static_cast<float>( subj[7])),

                hand2    ( static_cast<float>( subj[8]),  
                           static_cast<float>( subj[9]));*/
      
      //for each active subject
      for (auto ite = active_sub.begin(); ite != active_sub.end(); ) {
        
        //saving the graphs if the time life is over
        if (ite->old_ == 0) {
          ite->save2file(out_file);
          auto er = ite;
          ite++;
          active_sub.erase(er);
        }
        else {
          TrkPoint  prd = ite->trk_.predict();
          auto dist = cv::norm(prd - subCenter);
          if (dist < distance_sub_thr) {
            mindist = dist;
            nearest = ite;
          }
          ite->visited = true;
          ite++;
        }
      }

      if (nearest == active_sub.end()) {
        active_sub.push_back(Actor());
        nearest--;
        nearest->runTrk(subCenter);
        nearest->graph_.addSubjectNode(frame.frameNumber);
      }
      else {
        nearest->graph_.addSubjectNode(frame.frameNumber);
        nearest->trk_.estimate(subCenter);
      }
      nearest->frame_ini_ = frame.frameNumber;
      //!!!!!!!!!!!!!!!!!!!!!!!
      //MAY CODING WITH THREADS
      
      //for each candidate update the object interaction
      for (auto & obj : frame.sub_obj[1]) {
        //!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
        //distance to hands
        TrkPoint  nw( static_cast<float>(obj[1]), static_cast<float>(obj[3])),
                  se( static_cast<float>(obj[2]), static_cast<float>(obj[4]));

        /*auto dist = min ( distance2object (hand1, nw, se),
                            distance2object (hand2, nw, se));*/

        auto dist = norm(subCenter - TrkPoint((nw.x + se.x) / 2.0, (nw.y + se.y) / 2));

        if (dist < distance_obj_thr) {
          nearest->graph_.addObjectRelation( static_cast<Actor::oDataType>(dist), obj[0]);
        }
      }
    }
    //updating the years 
    for (auto & it : active_sub) {
      if (it.visited) {
        it.old_ = time_life;
        it.visited = false;
      }
      else
        --it.old_;
    }
  }  
  for (auto & it : active_sub) {
    it.save2file(out_file);
  }
}
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

void AnomalieControl::graphComparison() {
  
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
void AnomalieControl::graphDescription(string file, bool visual) {
  
}
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////


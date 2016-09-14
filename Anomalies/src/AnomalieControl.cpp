#include "AnomalieControl.h"

///default constructor
AnomalyControl::AnomalyControl(){}
///constructor string
AnomalyControl::AnomalyControl(string file):
main_prop_file_(file){
  
  string main_object_file;

  //reading the main file settings

  fs_main_.open(file, FileStorage::READ);
  assert(fs_main_.isOpened());
  fs_main_["main_object_file"]  >> main_object_file;
  fs_main_["main_frame_step"]   >> frame_step_;
  
  //loading objects
  loadDefinitions(main_object_file, objects_, objects_rev_);

  //loading functions

}
///default destructor
AnomalyControl::~AnomalyControl(){}
///
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
void AnomalyControl::run() {
  short op;
  fs_main_["main_operation"] >> op;
  switch (op)
  {
  case 1:
    graphBuilding();
    break;
  case 2:
    train();
    break;
  case 3:
    test();
    break;
  case 10:
    show();
    break;
  default:
    break;
  }
}


////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
void AnomalyControl::graphBuilding() {
 
  //variables..................................................................
  double  distance_obj_thr,
          distance_sub_thr;

  float   rze;

  int     time_life;

  string  out_dir,
          seq_file,
          out_token,
          video_file;

  double  jaccard;

  
  //............................................................................
   
  fs_main_["featExtract_seq_file"]          >> seq_file;
  fs_main_["featExtract_distance_sub_thr"]  >> distance_sub_thr;
  fs_main_["featExtract_distance_obj_thr"]  >> distance_obj_thr;
  fs_main_["featExtract_time_life"]         >> time_life;
  fs_main_["featExtract_out_dir"]           >> out_dir;
  fs_main_["featExtract_out_token"]         >> out_token;
  fs_main_["featExtract_jaccard"]           >> jaccard;
  
  fs_main_["show_video_file"]               >> video_file;
  fs_main_["show_resize"]                   >> rze;
  

  //............................................................................
  
  auto str = out_dir + cutil_LastName(seq_file) + out_token;
  graphBuild( seq_file, video_file, str, rze, distance_obj_thr, distance_sub_thr, 
              time_life, jaccard);

  
}
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

void AnomalyControl::show() {
  string  seq_file,
          video_file,
          graph_file;

  float   rze;

  list<FrameItem> frame_list;

  graphLstT  graphs;
  
  //............................................................................
  fs_main_["show_seq_file"]   >> seq_file;
  fs_main_["show_video_file"] >> video_file;
  fs_main_["show_resize"]     >> rze;
  fs_main_["show_graph_file"] >> graph_file;
  
  //............................................................................
  loadDescribedGraphs(graph_file, graphs, nullptr);
  show_graph( graphs, video_file, seq_file, rze, 0);
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
void AnomalyControl::train() {
  string      command,
              setup_file;
  
  vector<pf>  functions{
                    trainLevel1, //0 - first training
                    trainLevel2  //1 - second level
                  };

  graphLstT graphs;
  //............................................................................
  fs_main_["train_command"]     >> command;
  fs_main_["train_setup_file"]  >> setup_file;

  //............................................................................
  executeFunctionVec( functions, command, setup_file, frame_step_, 
                      &objects_, &objects_rev_, graphs);
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
void AnomalyControl::test() {
  
  string      command,
              setup_file;
  
  graphLstT   graphs;

  vector<pf>  test{
                    testLevel1,//0 first
                    testLevel2,//1 second
                    testLevel3 //2 third
  };

  //............................................................................
  fs_main_["test_command"]     >> command;
  fs_main_["test_setup_file"]  >> setup_file;

  //............................................................................
  executeFunctionVec( test, command, setup_file, frame_step_, 
                      &objects_, &objects_rev_, graphs);
  
  show_loadedGraph(setup_file, graphs);
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
/////////////////////SECONDARY FUNTIONS/////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
void AnomalyControl::show_loadedGraph(string &setup_file, graphLstT  &graphs) {
  string  seq_file,
          video_file,
          graph_file;

  float   rze;
  
  bool    flag = false;
  //............................................................................
  FileStorage fs(setup_file, FileStorage::READ);
  fs["show_flag"]       >> flag;
  fs["show_seq_file"]   >> seq_file;
  fs["show_video_file"] >> video_file;
  fs["show_resize"]     >> rze;
  
  //............................................................................
  if(flag)
    show_graph(graphs, video_file, seq_file, rze, 0);
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
void AnomalyControl::show_graph(  graphLstT   &graphs, string  &video_file,
                                  string      &seq_file, float   rze,
                                  int         anom_type) {

  Mat       img;

  Sequence  seq(frame_step_, seq_file, video_file, rze, objects_, objects_rev_);

  Scalar    RED(0, 0, 255), GREEN(0, 255, 0);

  //............................................................................
  //for each frame 
  for (auto & frm : seq.frames_) {
    cout << frm.first << endl;
    //getting image by request
    seq.getImage(frm.first, img);
    for (auto ite = graphs.begin(); ite != graphs.end(); ++ite) {

      if (ite->graph_.listNodes_.begin() != ite->graph_.listNodes_.end() &&
        ite->graph_.listNodes_.begin()->data_.id_ == frm.first) {

        auto head = ite->graph_.listNodes_.begin();
        stringstream subname;
        subname << "Subject " << ite->id_;

        seq.drawSub(frm.first, head->data_.list_idx_, img,
          ite->levels_[anom_type] ? RED : GREEN,
          subname.str());

        //for each object in the specific frame
        for (auto &ob : head->objectList_) {
          seq.drawObj(frm.first, ob.first.data_.list_idx_, img,
            ite->levels_[anom_type] ? RED : GREEN,
            subname.str());
        }
        //updateing graph
        ite->graph_.listNodes_.pop_front();
      }
      else {
        if (ite->graph_.listNodes_.begin() == ite->graph_.listNodes_.end() &&
          graphs.size() > 1 && ite != graphs.begin()) {
          auto it = ite;
          --ite;
          graphs.erase(it);
        }
      }
    }
    imshow("Frame", img);
    char flag;
    if (waitKey(30) >= 0) {
      char c = waitKey();
      if (c >= 30)break;
    }
  }
}


////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

void AnomalyControl::graphBuild( string  &seq_file, 
                                  string  &video_file,
                                  string  &out_file,
                                  float   rze,
                                  double  distance_obj_thr, 
                                  double  distance_sub_thr,
                                  int     time_life,
                                  double  jaccard) {
  list< Actor >   active_sub;

  stringstream    ss;

  int graph_count = 0;

  Mat img;

  Sequence        seq(  frame_step_, seq_file, video_file, rze, 
                        objects_, objects_rev_);

  //............................................................................
  //Tracking and looking for subjects
  //for each frame in parsed video
  for (auto & frame : seq.frames_) {
    cout << frame.first << endl;
    //seq.getImage(frame.first, img);

    //for each candidate subject update the list 
    int sub_pos = 0;
    for (auto & subj : frame.second.subjects_) {

      double    mindist = FLT_MAX;
      auto      nearest = active_sub.end();

      for (auto ite = active_sub.begin(); ite != active_sub.end();) {

        //saving the graphs if the time life is over
        if (ite->old_ == time_life) {
          ite->save2file(ss);
          auto er = ite;
          ++ite;
          active_sub.erase(er);
        }
        else {
          if (!ite->picked) {
            //....................................................................
            //in this part we take acount the distance and the jaccard
            Point prd = ite->trk_.predict();

            auto dist = cv::norm(prd - subj.center());
            auto jacc = jaccardBbox(  ite->sw_, ite->ne_,
                                      subj.sw_, subj.ne_);
            
            //in case of jaccard ou distance achieves with condition the it will 
            //be considered as a candidate
          
            if (dist < distance_sub_thr || jacc > jaccard) {
              mindist = dist;
              nearest = ite;
            }
          }

          //....................................................................
          ++ite;
        }
      }

      if (nearest == active_sub.end()) {
        active_sub.push_back(Actor());
        nearest--;
        nearest->runTrk(subj.center());
        nearest->id_ = graph_count++;
        nearest->graph_.addSubjectNode(ActorLabel(frame.first, sub_pos));
      }
      else {
        nearest->visited  = true;
        nearest->picked   = true;
        nearest->graph_.addSubjectNode(ActorLabel(frame.first, sub_pos));
        TrkPoint center = subj.center();
        nearest->trk_.estimate(center);
      }
      nearest->ne_ = subj.ne_;
      nearest->sw_ = subj.sw_;
      
      //seq.drawSub(subj, img, Scalar(100, 150, 255), "");

      //!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
      //MAY CODING WITH THREADS
      //........................................................................
      //for each candidate update the object interaction
      int obj_pos = 0;
      for (auto & obj : frame.second.objects_) {
        //distance to hands
        auto dist = min(  distance2object(subj.h1_, obj.sw_, obj.ne_),
                          distance2object(subj.h2_, obj.sw_, obj.ne_));

        //seq.drawObj(obj, img, Scalar(0, 255, 0), "");

        if (dist < distance_obj_thr) {
          nearest->graph_.addObjectRelation(ActorLabel(obj.id_, obj_pos), (dist<0 ? 0 : dist));
        }
        ++obj_pos;
      }
      ++sub_pos;
    }
    //updating the years 
    for (auto & it : active_sub) {
      if (it.visited) {
        it.old_     = 0;
        it.visited  = false;
        it.picked   = false;
      }
      else
        ++it.old_;
    }
  }
  
  for (auto & it : active_sub) 
    it.save2file(ss);

  //saving into file
  ofstream arc(out_file);
  cout << "Saving Graph in " << out_file << endl;
  arc << ss.str();
  arc.close();
}

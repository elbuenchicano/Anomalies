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
    training();
    break;
  case 3:
    testing1();
    break;
  case 4:
    testing2();
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

  
  //............................................................................
   
  fs_main_["featExtract_seq_file"]          >> seq_file;
  fs_main_["featExtract_distance_sub_thr"]  >> distance_sub_thr;
  fs_main_["featExtract_distance_obj_thr"]  >> distance_obj_thr;
  fs_main_["featExtract_time_life"]         >> time_life;
  fs_main_["featExtract_out_dir"]           >> out_dir;
  fs_main_["featExtract_out_token"]         >> out_token;

  fs_main_["show_video_file"]               >> video_file;
  fs_main_["show_resize"]                   >> rze;
  

  //............................................................................
  
  auto str = out_dir + cutil_LastName(seq_file) + out_token;
  graphBuild( seq_file, video_file, str, rze, distance_obj_thr, distance_sub_thr, 
              time_life);

  
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
  show(graphs, video_file, seq_file, rze, nullptr);
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
void AnomalyControl::train() {
  string      command,
              setup_file;
  
  vector<pf>  test;

  //............................................................................
  fs_main_["train_command"]     >> command;
  fs_main_["train_setup_file"]  >> setup_file;

  //............................................................................
  executeFunctionVec( test, command, setup_file, frame_step_, 
                      &objects_, &objects_rev_);
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
void AnomalyControl::training() {
  string  token,
          directory,
          out_voc_file,
          out_dist_file,
          out_obs_file;

  cutil_file_cont graph_files;

  graphLstT       graph_list;

  set<int>        observedObjs;

  //............................................................................
  fs_main_["training_token"]        >> token;
  fs_main_["training_directory"]    >> directory;
  fs_main_["training_out_voc_file"] >> out_voc_file;
  fs_main_["training_out_obs_file"] >> out_obs_file;
  fs_main_["training_out_dist_file"]>> out_dist_file;



  //............................................................................
  //First level: atomic anomalies 
  //dictionary build 
  cout << "Training...\n";
  list_files(graph_files, directory.c_str(), token.c_str());
  for (auto & file : graph_files) {
    cout << file << endl;
    loadDescribedGraphs(file, graph_list, &observedObjs);
  }

  //............................................................................
  //saving vocabulary in file
  //set<string> voc;
  //dictionaryBuild(graph_list, out_voc_file, voc);
  

  //............................................................................
  //recording the distributions
  //distributions(graph_list, out_dist_file, voc);

  //............................................................................
  //saving the observed objects
  ofstream arc(out_obs_file);
  cout << "Observed ojects saved in: \n" << out_obs_file << endl;
  cutil_cont2os(arc, observedObjs, "\n");
  arc.close();
  
  map<string, int> voc;
  distributionBuild(graph_list, out_dist_file, observedObjs, voc);
  

}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
void AnomalyControl::test() {
  
  string      command,
              setup_file;
  
  vector<pf>  test;

  //............................................................................
  fs_main_["test_command"]     >> command;
  fs_main_["test_setup_file"]  >> setup_file;

  //............................................................................
  executeFunctionVec( test, command, setup_file, frame_step_, 
                      &objects_, &objects_rev_);
  
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

void AnomalyControl::testing1() {
  string  voc_file,
          graph_file,
          out_file;
  
  int     pos;

  graphLstT  graph_test;

  //............................................................................
  fs_main_["testing1_voc_file"]    >> voc_file;
  fs_main_["testing1_graph_file"]  >> graph_file;
  fs_main_["testing1_out_file"]    >> out_file;
  
  //............................................................................
  loadDescribedGraphs(graph_file, graph_test, nullptr);
  
  //............................................................................
  //first level anomalie recognition 
  ofstream arc(out_file);
  auto voc = cutil_load2strv(voc_file);
  for (auto &graph : graph_test) {
    for (auto &node : graph.graph_.listNodes_) {
      set<int> objects;
      for (auto &par : node.objectList_)
        objects.insert(par.first.data_.id_);
      auto  str = set2str(objects);

      if (algoUtil_bin_search<string>(voc, str, pos)) {
        arc << "Warning: Frame" << graph.graph_.listNodes_.begin()->data_.id_ << "/n";
        arc << "Unknown word: " << str << "/n";
      }
    }
  }
  arc.close();
  //TO DO: present in images 
 

}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

void AnomalyControl::testing2() {
  string  histo_file,
          graph_file,
          obs_file,
          out_file;

  graphLstT  graph_test;
  
  Mat_<int> histos;

  double    thr;

  bool      visual;
  //............................................................................
  
  fs_main_["testing2_histo_file"] >> histo_file;
  fs_main_["testing2_graph_file"] >> graph_file;
  fs_main_["testing2_out_file"]   >> out_file;
  fs_main_["testing2_obs_file"]   >> obs_file;
  fs_main_["testing2_thr"]        >> thr;
  fs_main_["testing2_visual"]     >> visual;

  


  //............................................................................
  loadDescribedGraphs(graph_file, graph_test, nullptr);
  FileStorage fs(histo_file, FileStorage::READ);
  fs["Histograms"] >> histos;

  //............................................................................
  //second level anomalie recognition
  auto vstr = cutil_load2strv(obs_file);
  set<int> obsO;
  for (auto &it : vstr)
    obsO.insert(stoi(it));

  map<string, int> voc;
  distributionPermutation(obsO, voc);
  
  Mat_<int>     line;
  double        dist, 
                min = FLT_MAX;
  stringstream  wrg;

  list<bool>    anomalyList;

  for (auto &graph : graph_test) {
    auto h = distribution(graph.graph_, voc);
    for (int i = 0; i < histos.rows; ++i) {
      line = histos.row(i);
      dist = norm(h, line);
      if (dist < min) min = dist;
    }
    if (dist > thr) {
      wrg << "Warning Graph!" << graph.graph_.listNodes_.begin()->data_.id_ << endl;
      anomalyList.push_back(true);
    }
    else
      anomalyList.push_back(false);
  }

  //............................................................................
  //savin in output file
  cout << wrg.str();
  
  if (out_file != "") {
    ofstream arc(out_file);
    cout << "Warnings saved in: " << out_file << endl;
    arc << wrg.str();
    arc.close();
  }

  //............................................................................
  //show anomalies in screen image sequence

  if (visual) {
    string  video_file,
            seq_file;
    float   rze;
    fs_main_["show_seq_file"]   >> seq_file;
    fs_main_["show_video_file"] >> video_file;
    fs_main_["show_resize"]     >> rze;
    
    show(graph_test, video_file, seq_file, rze, &anomalyList);
  }

}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
/////////////////////SECONDARY FUNTIONS/////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
void AnomalyControl::show( graphLstT   &graphs,    string  &video_file, 
                            string      &seq_file,  float   rze,
                            list<bool>  *anomalies) {
  Mat       img;

  Sequence  seq(frame_step_, seq_file, video_file, rze, objects_, objects_rev_);

  
  //............................................................................
  //for each frame 
  for (auto & frm : seq.frames_) {
    //getting image by request
    seq.getImage(frm.first, img);
    for (auto ite = graphs.begin(); ite != graphs.end(); ++ite) {

      if (  ite->graph_.listNodes_.begin() != ite->graph_.listNodes_.end() &&
            ite->graph_.listNodes_.begin()->data_.id_ ==  frm.first){ 
        
        auto head = ite->graph_.listNodes_.begin();
        stringstream subname;
        subname << "Subject " << ite->id_;
        seq.drawSub( frm.first, head->data_. list_idx_,img, 
                     Scalar(0,0,255), subname.str());
        
        //for each object in the specific frame
        for (auto &ob : head->objectList_) {
          seq.drawObj( frm.first, ob.first.data_.list_idx_, img, 
                       Scalar(0, 255, 0), subname.str());
        }
        //updateing graph
        ite->graph_.listNodes_.pop_front();
      }
      else {
        if (  ite->graph_.listNodes_.begin() == ite->graph_.listNodes_.end() &&
              graphs.size() > 1 ) {
          auto it = ite;
          --ite;
          graphs.erase(it);
        }
      }
    }
    imshow("Frame", img);
    if (waitKey(30) >= 0) break;
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
                                  int     time_life) {
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
          Point prd = ite->trk_.predict();
          auto dist = cv::norm(prd - subj.center());
          if (dist < distance_sub_thr && !ite->picked) {
            mindist = dist;
            nearest = ite;
          }
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

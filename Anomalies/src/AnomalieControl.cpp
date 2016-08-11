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
  loadDefinitions(main_object_file, objects_, objects_rev_);

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
void AnomalieControl::graphBuilding() {
 
  
  //variables..................................................................
  list< Actor >     active_sub;

  list<FrameItem>   frame_list;

  double            distance_obj_thr,
                    distance_sub_thr;

  int               time_life;

  string            out_file,
                    seq_file,
                    out_token;

  
  //............................................................................
   
  fs_main_["featExtract_seq_file"]          >> seq_file;
  fs_main_["featExtract_distance_sub_thr"]  >> distance_sub_thr;
  fs_main_["featExtract_distance_obj_thr"]  >> distance_obj_thr;
  fs_main_["featExtract_time_life"]         >> time_life;
  fs_main_["featExtract_out_file"]          >> out_file;
  fs_main_["featExtract_out_token"]         >> out_token;

  //............................................................................
  loadFrameList(seq_file, frame_list, frame_step_, objects_);
  out_file += cutil_LastName(seq_file) + out_token;

  //............................................................................
  //Tracking and looking for subjects
  //for each frame in parsed video
  for (auto & frame : frame_list) {
    cout << frame.frameNumber<< endl;
    //auto img = show(frame.frameNumber);

    //for each candidate subject update the list 
    int sub_pos = 0;
    for (auto & subj : frame.sub_obj[0]) {
     
      double    mindist = FLT_MAX;
      auto      nearest = active_sub.end();

      TrkPoint  subCenter( static_cast<float>( (subj[1] + subj[3]) / 2.0), 
                           static_cast<float>( (subj[2] + subj[4]) / 2.0) );

      //////////////////////////////////////////////////////////////////////
      //forget this part when bug is fixed
      
      TrkPoint  hand1    ( static_cast<float>( subj[6]),  
                           static_cast<float>( subj[7])),

                hand2    ( static_cast<float>( subj[8]),  
                           static_cast<float>( subj[9]));
      

      //circle(img, hand1, 5, Scalar(255, 0, 255), -1, 8);
      //circle(img, hand2, 5, Scalar(255, 0, 255), -1, 8);

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
          if (dist < distance_sub_thr && !ite->picked) {
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
        nearest->frame_ini_ = frame.frameNumber;
        nearest->graph_.addSubjectNode(ActorLabel(frame.frameNumber, sub_pos));
      }
      else {
        nearest->picked = true;
        nearest->graph_.addSubjectNode(ActorLabel(frame.frameNumber, sub_pos));
        nearest->trk_.estimate(subCenter);
      }
      //!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
      //MAY CODING WITH THREADS
      //........................................................................
      //for each candidate update the object interaction
      int obj_pos = 0;
      for (auto & obj : frame.sub_obj[1]) {
        //!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
        //distance to hands

        

        TrkPoint  sw( static_cast<float>(obj[1]), static_cast<float>(obj[2])),
                  ne( static_cast<float>(obj[3]), static_cast<float>(obj[4]));

        auto dist = min ( distance2object (hand1, sw, ne),
                          distance2object (hand2, sw, ne));


        //rectangle(img, nw, se, Scalar(0, 150, 255));

        //auto dist2 = norm(subCenter - TrkPoint((nw.x + se.x) / 2.0, (nw.y + se.y) / 2));

        if (dist < distance_obj_thr) {
          nearest->graph_.addObjectRelation(ActorLabel(obj[0], obj_pos), (dist<0?0:dist) );
        }
        ++obj_pos;
      }
      ++sub_pos;
    }
    //updating the years 
    for (auto & it : active_sub) {
      if (it.visited) {
        it.old_ = time_life;
        it.visited = false;
        it.picked  = false;
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

void AnomalieControl::show() {
  string  seq_file,
          video_file,
          graph_file;

  float   rze;

  list<FrameItem>   frame_list;

  graphLstT         graphs;
  
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

void AnomalieControl::training() {
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

void AnomalieControl::testing1() {
  string  voc_file,
          graph_file,
          out_file;
  int     pos;
  graphLstT graph_test;
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
    for (auto &node : graph.listNodes_) {
      set<int> objects;
      for (auto &par : node.objectList_)
        objects.insert(par.first.data_.id_);
      auto  str = set2str(objects);

      if (algoUtil_bin_search<string>(voc, str, pos)) {
        arc << "Warning: Frame" << graph.listNodes_.begin()->data_.id_ << "/n";
        arc << "Unknown word: " << str << "/n";
      }
    }
  }
  arc.close();
  //TO DO: present in images 
 

}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

void AnomalieControl::testing2() {
  string  histo_file,
          graph_file,
          obs_file,
          out_file;

  graphLstT graph_test;
  
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
    auto h = distribution(graph, voc);
    for (int i = 0; i < histos.rows; ++i) {
      line = histos.row(i);
      dist = norm(h, line);
      if (dist < min) min = dist;
    }
    if (dist > thr) {
      wrg << "Warning Graph!" << graph.listNodes_.begin()->data_.id_ << endl;
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
    double  rze;
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

Mat AnomalieControl::show(int frame) {
  
  string  video_file,
          rze;
  fs_main_["show_video_file"] >> video_file;
  fs_main_["show_resize"]     >> rze;
  
  mfVideoSequence seq(video_file);
  assert(seq.cap_.isOpened());
  Mat img;
  seq.getImage(img, frame);
  return img;
}



////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

void AnomalieControl::dictionaryBuild(graphLstT &lst, string &out_file, 
                                      set<string> & voc) {
  
  voc.clear();
  
  for (auto &graph : lst){
    for (auto &node : graph.listNodes_) {
      set<int> objects; 
       
      for (auto &par : node.objectList_) {
        objects.insert(par.first.data_.id_);
      }
      auto  str = set2str(objects);
      if(str != "")
        voc.insert(str);
    }
  }
  ofstream arc(out_file);
  cout << "Written in " << out_file << endl;
  cutil_cont2os(arc, voc, "\n");
  arc.close();
}
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

void AnomalieControl::listObservedObjs( graphLstT &lst, string &out_file,
                                        set<int> &voc){
  voc.clear();

  for (auto &graph : lst)
    for (auto &node : graph.listNodes_) 
      for (auto &par : node.objectList_) 
        voc.insert(par.first.data_.id_);
  ofstream arc(out_file);
  cout << "Written in " << out_file << endl;
  cutil_cont2os(arc, voc, "\n");
  arc.close();
}


////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

void AnomalieControl::distributions(  graphLstT &lst, string &out_file,
                                      set<string> & voc) {
  map <string, int> dist;

  //initalizing with zero values
  for (auto & it : voc)
    dist[it] = 0;

  Mat_<int> histograms;
  //creating for each graph
  for (auto &graph : lst) {
    auto h = graphHistrogram(graph, dist, voc);
    histograms.push_back(h);
  }

  FileStorage fs(out_file, FileStorage::WRITE);
  cout  << "Written in "  << out_file << endl;
  fs    << "Histograms"   << histograms;


}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

void AnomalieControl::distributionBuild(  graphLstT   &lst, string &out_file,
                                          set<int>    &obsO,
                                          map<string,int> & voc) {

  voc.clear();
  distributionPermutation(obsO, voc);

  Mat_<int> histograms;
  for (auto &graph : lst) {
    auto h = distribution(graph, voc);
    histograms.push_back(h);
  }

  //saving to file 

  FileStorage fs(out_file, FileStorage::WRITE);
  cout  << "Written in " << out_file << endl;
  fs    << "Histograms" << histograms;
}

Mat_<int> AnomalieControl::distribution ( graphType    &graph,
                                          map<string, int>  &dist){
  
  Mat_<int> histo(1, static_cast<int>(dist.size()));
  histo = histo * 0;
  vector<string> tmp;

  for (auto &node : graph.listNodes_) {
    set<int> objects;

    for (auto &par : node.objectList_) {
      objects.insert(par.first.data_.id_);
    }
    auto  str = set2str(objects);
    tmp = cutil_string_split(str);

    if (tmp.size() > 1) {
      for (auto &it : tmp)
        ++histo(0, dist[it]);
    }
    ++histo(0, dist[str]);  
  }

  return histo;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
void AnomalieControl::show( graphLstT   &graphs,    string  &video_file, 
                            string      &seq_file,  float   rze,
                            list<bool>  *anomalies) {
  Mat   img;

  int   gcont;

  list<FrameItem>   frame_list;
  
  list<bool>::iterator ait;

  //............................................................................
  mfVideoSequence seq(video_file);
  loadFrameList(seq_file, frame_list, frame_step_, objects_);
  
  //............................................................................
  
  //begins in the listframe and the same time extracts the image by framenumber
  for (auto frm = frame_list.begin(); frm != frame_list.end() &&
    seq.getImage(img, frm->frameNumber); frm++) {

    //drawing frame number
    stringstream lblframe;
    lblframe << "Frame " << frm->frameNumber;
    putText(img, lblframe.str(), Point(0, 20), CV_FONT_HERSHEY_PLAIN, 2.0, Scalar(0, 0, 255));

    //..........................................................................
    //for each graph
    gcont = 0;

    //if vector anomalies is on
    if (anomalies)
      ait = anomalies->begin();

    for (auto graphs_ite = graphs.begin(); graphs_ite != graphs.end(); graphs_ite++) {

      //resizes the image given the case  
      if (rze > 0)resize(img, img, Size(), rze, rze);

      if (graphs_ite->listNodes_.begin() != graphs_ite->listNodes_.end() &&
        graphs_ite->listNodes_.begin()->data_.id_ == frm->frameNumber) {

        auto head = graphs_ite->listNodes_.begin();
        Point p1(frm->sub_obj[0][head->data_.list_idx_][1],
          frm->sub_obj[0][head->data_.list_idx_][2]);

        Point p2(frm->sub_obj[0][head->data_.list_idx_][3],
          frm->sub_obj[0][head->data_.list_idx_][4]);

        Point hand1(frm->sub_obj[0][head->data_.list_idx_][6],
          frm->sub_obj[0][head->data_.list_idx_][7]);

        Point hand2(frm->sub_obj[0][head->data_.list_idx_][8],
          frm->sub_obj[0][head->data_.list_idx_][9]);

        //drawing subject
        rectangle(img, p1, p2, Scalar(255, 0, 0));

        //drawing hands

        circle(img, hand1, 5, Scalar(255, 0, 255), -1, 8);
        circle(img, hand2, 5, Scalar(255, 0, 255), -1, 8);

        //put label
        stringstream sublbl;
        sublbl << "Sub" << gcont;
        putText(img, sublbl.str(), p1, CV_FONT_HERSHEY_PLAIN, 1.0, Scalar(255, 0, 0));

        for (auto &ob : head->objectList_) {
          Point p1(frm->sub_obj[1][ob.first.data_.list_idx_][1],
            frm->sub_obj[1][ob.first.data_.list_idx_][2]);

          Point p2(frm->sub_obj[1][ob.first.data_.list_idx_][3],
            frm->sub_obj[1][ob.first.data_.list_idx_][4]);

          //drawing rectangle
          stringstream sublbl;
          sublbl << objects_rev_[ob.first.data_.id_] << " Sub" << gcont;
          rectangle(img, p1, p2, Scalar(0, 150, 255));
          putText(img, sublbl.str(), p1, CV_FONT_HERSHEY_PLAIN, 1.0, Scalar(0, 150, 255));
        }

        graphs_ite->listNodes_.pop_front();
      }
      if(anomalies)++ait;
    }
    cv::imshow("Frame", img);
    //if (waitKey(30) >= 0) break;
    cv::waitKey();
    ++gcont;
  }



}
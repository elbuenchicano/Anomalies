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
    //for each candidate subject update the list 
    int sub_pos = 0;
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
        nearest->frame_ini_ = frame.frameNumber;
        nearest->graph_.addSubjectNode(ActorLabel(frame.frameNumber, sub_pos));
      }
      else {
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
        TrkPoint  nw( static_cast<float>(obj[1]), static_cast<float>(obj[3])),
                  se( static_cast<float>(obj[2]), static_cast<float>(obj[4]));

        /*auto dist = min ( distance2object (hand1, nw, se),
                            distance2object (hand2, nw, se));*/

        auto dist = norm(subCenter - TrkPoint((nw.x + se.x) / 2.0, (nw.y + se.y) / 2));

        if (dist < distance_obj_thr) {
          nearest->graph_.addObjectRelation(ActorLabel(obj[0], obj_pos),dist);
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

  double  rze;

  list<FrameItem>   frame_list;

  graphLstT         graphs;

  typedef BaseDefinitions_tr::graphType::lstNodeType lstNodeType;

  //............................................................................
  fs_main_["show_seq_file"]   >> seq_file;
  fs_main_["show_video_file"] >> video_file;
  fs_main_["show_resize"]     >> rze;
  fs_main_["show_graph_file"] >> graph_file;

  //............................................................................
  mfVideoSequence seq(video_file);
  loadFrameList(seq_file, frame_list, frame_step_, objects_);

  loadDescribedGraphs(graph_file, graphs);

  //............................................................................
  Mat   img;
  int   gcont;
  for (auto frm = frame_list.begin(); frm != frame_list.end() &&
    seq.getImage(img, frm->frameNumber); frm++) {

    //for each graph
    gcont = 0;
    for (auto graphs_ite = graphs.begin(); graphs_ite != graphs.end(); graphs_ite++) {

      if (rze > 0)resize(img, img, Size(), rze, rze);
      if (graphs_ite->listNodes_.begin() != graphs_ite->listNodes_.end() &&
        graphs_ite->listNodes_.begin()->data_.id_ == frm->frameNumber) {

        auto head = graphs_ite->listNodes_.begin();
        Point p1(frm->sub_obj[0][head->data_.list_idx_][1],
          frm->sub_obj[0][head->data_.list_idx_][2]);

        Point p2(frm->sub_obj[0][head->data_.list_idx_][3],
          frm->sub_obj[0][head->data_.list_idx_][4]);

        //drawing subject
        rectangle(img, p1, p2, Scalar(255, 0, 0));
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

    }
    imshow("Frame", img);
    if (waitKey(30) >= 0) break;
    //cv::waitKey();
    ++gcont;
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

void AnomalieControl::training(){
  string  token,
          directory,
          out_voc_file;

  cutil_file_cont graph_files;

  graphLstT       graph_list;

  //............................................................................
  fs_main_["training_token"]          >> token;
  fs_main_["training_directory"]      >> directory;
  fs_main_["training_out_voc_file"]   >> out_voc_file;

  
  //............................................................................
  list_files(graph_files, directory.c_str(), token.c_str());
  for (auto & file : graph_files) {
    loadDescribedGraphs(file, graph_list);
  }
  dictionaryBuild(graph_list, out_voc_file);
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

void AnomalieControl::testing() {
  string  voc_file;
  string  graph_file;
  int     pos;
  graphLstT graph_test;
  //............................................................................
  fs_main_["testing_voc_file"]    >> voc_file;
  fs_main_["testing_graph_file"]  >> graph_file;

  //............................................................................

  loadDescribedGraphs(graph_file, graph_test);
  auto voc = cutil_load2strv(voc_file);
  for (auto &graph : graph_test) {
    for (auto &node : graph.listNodes_) {
      set<int> objects;
      for (auto &par : node.objectList_)
        objects.insert(par.first.data_.id_);
      auto  str = set2str(objects);
   
      if (cutil_bin_search<string>(voc, str, pos)) {
        cout << "Warning: Frame" << graph.listNodes_.begin()->data_.id_<<"/n";
        cout << "Unknown word: " << str << "/n" ;
      }
    }
  }


}



////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

void AnomalieControl::dictionaryBuild(graphLstT &lst, string &out_file) {
  
  set<string> voc;
  for (auto &graph : lst){
    for (auto &node : graph.listNodes_) {
      set<int> objects;
      for (auto &par : node.objectList_)
        objects.insert(par.first.data_.id_);
      auto  str = set2str(objects);
      voc.insert(str);
    }
  }
  ofstream arc(out_file);
  cutil_cont2os(arc, voc, "\n");
  arc.close();
}
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////


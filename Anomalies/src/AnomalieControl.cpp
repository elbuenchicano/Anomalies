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
          graph_file,
          out_file;

  float   rze;
  
  int     flag;

  list<FrameItem> frame_list;

  graphLstT  graphs;
  
  //............................................................................
  fs_main_["show_seq_file"]   >> seq_file;
  fs_main_["show_video_file"] >> video_file;
  fs_main_["show_resize"]     >> rze;
  fs_main_["show_graph_file"] >> graph_file;
  fs_main_["show_out_file"]   >> out_file;
  fs_main_["show_flag"] >> flag;
  //............................................................................
  switch (flag)
  {
  case 0: {
    loadDescribedGraphs(graph_file, graphs, nullptr);
    show_graph(graphs, video_file, seq_file, rze, 0);
    break;
  }
  case 1: {
    show_seq(video_file, seq_file, rze);
  }
  case 2: {
    show_hand(video_file, seq_file, out_file, rze);
  }
  case 3: {
    show_hand_(video_file, seq_file, out_file, rze);
  }
  default:
    break;
  }
  
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
void AnomalyControl::train() {
  string      command,
              setup_file;
  
  vector<pf>  functions{
                    trainLevel1, //0 - first training
                    trainLevel2, //1 - second level
                    trainLevel4  //3 = fourth level 
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
                    testLevel3,//2 third
                    testLevel4 //3 fourth
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

  Scalar    RED(0, 0, 255), GREEN(0, 255, 0), BLUE(255, 0, 0);

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
          //ite->levels_[anom_type] ? RED : GREEN,
          BLUE,
          subname.str());

        //for each object in the specific frame
        for (auto &ob : head->objectList_) {
          seq.drawObj(frm.first, ob.first.data_.list_idx_, img,
            ob.first.data_.anomaly_ ? RED : GREEN,
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
    if (waitKey(30) >= 0) {
      char c = waitKey();
      if (c >= 30)break;
    }
  }
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
void AnomalyControl::show_seq(  string      &       video_file,
                                string      &       seq_file,
                                float               rze){
  
  Mat       img;

  Sequence  seq(frame_step_, seq_file, video_file, rze, objects_, objects_rev_);

  int       pos = 0;

  //............................................................................
  //for such frame
  cout << "Video frame length " << seq.frames_.size() * frame_step_ << endl;
  cout << "Give the star frame\n";
  cin >> pos;
  if (pos < 0) pos = 0;
  while (pos <= seq.frames_.rbegin()->first) {
    cout << "Frame number: " << pos << endl;
    seq.getImage(pos, img);
    seq.drawAllObjs(pos, img);
    seq.drawAllSubs(pos, img);
    imshow("Frame", img);
    char c = waitKey();
    if (c == 'q')break;
    pos += frame_step_;
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
      nearest = active_sub.end();
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



////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////



Mat src, Img_gt, ROI;
Rect cropRect(0, 0, 0, 0);
Point P1(0, 0);
Point P2(0, 0);

const char* winName = "Crop Image";
bool clicked = false;
int i = 0;
char Img_gtName[15];


void checkBoundary() {
  //check croping rectangle exceed image boundary
  if (cropRect.width>Img_gt.cols - cropRect.x)
    cropRect.width = Img_gt.cols - cropRect.x;

  if (cropRect.height>Img_gt.rows - cropRect.y)
    cropRect.height = Img_gt.rows - cropRect.y;

  if (cropRect.x<0)
    cropRect.x = 0;

  if (cropRect.y<0)
    cropRect.height = 0;
}

void showImage() {
  Img_gt = src.clone();
  checkBoundary();
  if (cropRect.width>0 && cropRect.height>0) {
    ROI = src(cropRect);
    imshow("cropped", ROI);
  }

  rectangle(Img_gt, cropRect, Scalar(0, 255, 0), 1, 8, 0);
  imshow(winName, Img_gt);
}


void onMouse(int event, int x, int y, int f, void*) {


  switch (event) {

  case  CV_EVENT_LBUTTONDOWN:
    clicked = true;

    P1.x = x;
    P1.y = y;
    P2.x = x;
    P2.y = y;
    break;

  case  CV_EVENT_LBUTTONUP:
    P2.x = x;
    P2.y = y;
    clicked = false;
    break;

  case  CV_EVENT_MOUSEMOVE:
    if (clicked) {
      P2.x = x;
      P2.y = y;
    }
    break;

  default:   break;


  }


  if (clicked) {
    if (P1.x>P2.x) {
      cropRect.x = P2.x;
      cropRect.width = P1.x - P2.x;
    }
    else {
      cropRect.x = P1.x;
      cropRect.width = P2.x - P1.x;
    }

    if (P1.y>P2.y) {
      cropRect.y = P2.y;
      cropRect.height = P1.y - P2.y;
    }
    else {
      cropRect.y = P1.y;
      cropRect.height = P2.y - P1.y;
    }

  }


  showImage();


}


void engine(){
  cout << "Click and drag for Selection" << endl << endl;
  cout << "------> Press 's' to save" << endl << endl;

  cout << "------> Press '8' to move up" << endl;
  cout << "------> Press '2' to move down" << endl;
  cout << "------> Press '6' to move right" << endl;
  cout << "------> Press '4' to move left" << endl << endl;

  cout << "------> Press 'w' increas top" << endl;
  cout << "------> Press 'x' increas bottom" << endl;
  cout << "------> Press 'd' increas right" << endl;
  cout << "------> Press 'a' increas left" << endl << endl;

  cout << "------> Press 't' decrease top" << endl;
  cout << "------> Press 'b' decrease bottom" << endl;
  cout << "------> Press 'h' decrease right" << endl;
  cout << "------> Press 'f' decrease left" << endl << endl;

  cout << "------> Press 'r' to reset" << endl;
  cout << "------> Press 'Esc' to quit" << endl << endl;


  namedWindow(winName, WINDOW_NORMAL);
  setMouseCallback(winName, onMouse, NULL);
  imshow(winName, src);

  while (1) {
    char c = waitKey();
    if (c == '6') cropRect.x++;
    if (c == '4') cropRect.x--;
    if (c == '8') cropRect.y--;
    if (c == '2') cropRect.y++;

    if (c == 'w') { cropRect.y--; cropRect.height++; }
    if (c == 'd') cropRect.width++;
    if (c == 'x') cropRect.height++;
    if (c == 'a') { cropRect.x--; cropRect.width++; }

    if (c == 't') { cropRect.y++; cropRect.height--; }
    if (c == 'h') cropRect.width--;
    if (c == 'b') cropRect.height--;
    if (c == 'f') { cropRect.x++; cropRect.width--; }

    if (c == 27) break;
    if (c == 'r') { cropRect.x = 0; cropRect.y = 0; cropRect.width = 0; cropRect.height = 0; }
    showImage();

  }

}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
void AnomalyControl::show_hand  ( string      &       video_file,
                                  string      &       seq_file,
                                  string      &       out_file,
                                  float               rze) {

  Mat       img;

  Sequence  seq(1, seq_file, video_file, rze, objects_, objects_rev_);

  int       pos = 0;

  //............................................................................
  //for such frame
  cout << "Video frame length " << seq.frames_.size() << endl;
  cout << "Give the star frame\n";
  cin >> pos;
  if (pos < 0) pos = 0;
  while (pos <= seq.frames_.rbegin()->first) {
    cout << "Frame number: " << pos << endl;
    seq.getImage(pos, img);
    //seq.drawAllObjs(pos, img);
    //seq.drawAllSubs(pos, img);
    //imshow("Frame", img);

    src = img.clone();
    engine();


    Point sw(cropRect.x, cropRect.y),
          ne(cropRect.x + cropRect.width, cropRect.y + cropRect.height);

    if (sw.x && sw.y && ne.x && ne.y) {
      cout << sw << " - " << ne << endl;

      seq.frames_[pos].objects_.push_back(ObjectItem(80, sw, ne));
    }
    char c = waitKey();
    if (c == 'q')break;
    pos += frame_step_;
  }
  seq.write2file(out_file);
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

void AnomalyControl::show_hand_(string      &       video_file,
  string      &       seq_file,
  string      &       out_file,
  float               rze) {

  Mat       img;

  Sequence  seq(1, seq_file, video_file, rze, objects_, objects_rev_);

  int       pos = 0;

  //............................................................................
  //for such frame
  cout << "Video frame length " << seq.frames_.size() << endl;
  cout << "Give the star frame\n";
  cin >> pos;
  if (pos < 0) pos = 0;
  while (pos <= seq.frames_.rbegin()->first) {
    cout << "Frame number: " << pos << endl;
    seq.getImage(pos, img);
    //seq.drawAllObjs(pos, img);
    seq.drawAllSubs(pos, img);
    //imshow("Frame", img);

    src = img.clone();
    engine();


    Point sw(cropRect.x, cropRect.y),
          ne(cropRect.x + cropRect.width, cropRect.y + cropRect.height);

    if (sw.x && sw.y && ne.x && ne.y) {
      cout << sw << " - " << ne << endl;
      Point mid = (sw + ne) / 2;
      if(seq.frames_[pos].subjects_.size()>0)
        seq.frames_[pos].subjects_[0].h1_ = mid;
    }
    char c = waitKey();
    if (c == 'q')break;
    pos += frame_step_;
  }
  seq.write2file(out_file);
}
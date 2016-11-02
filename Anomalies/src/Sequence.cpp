#include "Sequence.h"


Sequence::Sequence( short   step,
                    string  &desc_file,
                    string  &video_file,
                    float   rze,
                    map<string, int> &objs,
                    map<int, string> &objs_i):
step_(step),
desc_file_(desc_file),
rze_(rze),
video_file_(video_file){
  objs_   = &objs;
  objs_i_ = &objs_i;
  loadSequence();
  if (video_file_ != "") 
    video_ = make_shared<mfVideoSequence>(video_file_);
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//looking for especific line with tag frame
inline int lookingforFrame(ifstream &seq, list<string> &content ) {
  content.clear();
  for (string line; getline(seq, line); ) {
    auto vline = cutil_string_split(line);
    if (vline.size() > 1) {
      if (vline[0] == "Frame")
        return stoi(vline[1]);
    }
    content.push_back(line);
  }
  return -1;
}

////////////////////////////////////////////////////////////////////////////////
//getting info for specific line
void loadFrameItems(vector<string > &vline, FrameSeq &frm,
  map<string, int> &objs)
{
  //containing the id and positions 
  //string for person
  size_t      i, j;
  vector<int> vint;

  if (vline[0] == "person") {
    Point sw(stoi(vline[1]), stoi(vline[2]));
    Point ne(stoi(vline[3]), stoi(vline[4]));
     
    ne.x += sw.x;
    ne.y += sw.y;

    frm.subjects_.push_back(
      SubjectItem(  sw,
                    ne,
                    Point( stoi(vline[6]),stoi(vline[7]) ),
                    Point( stoi(vline[8]),stoi(vline[9]) )
                  )
      );
  }
  else {
    //in case of composed names
    j = vline.size() - 6;
    for (i = 1; i <= j; i++) {
      vline[0] += ' ' + vline[i];
    }
    Point sw(stoi(vline[j+1]), stoi(vline[j+2]));
    Point ne(stoi(vline[j+3]), stoi(vline[j+4]));

    ne.x += sw.x;
    ne.y += sw.y;

    frm.objects_.push_back( 
      ObjectItem(objs[vline[0]], sw, ne) 
      );
      
  }
}
////////////////////////////////////////////////////////////////////////////////
//loading sequences from file, must have person as subject and this format
//Frame <number>
//person x1, y1, w, h, score, h1_x, h1_y, h2_x, h2_y
//<object> x1, y1, w, h, score
void Sequence::loadSequence()
{
  ifstream seq(desc_file_);
  assert(seq.is_open());
  
  list<string> content;
  for (int  frmNumber = lookingforFrame(seq, content); seq && frmNumber >= 0; 
            frmNumber = lookingforFrame(seq, content)) {
    if ( (frmNumber-1) % step_ == 0) {
      FrameSeq frm;
      for (auto &conLine : content) {
        auto vline = cutil_string_split(conLine);
        loadFrameItems(vline, frm, *objs_);
      }
      frames_[frmNumber-1] = frm;
    }
  }
  seq.close();
}
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
void Sequence::getImage(int frame, Mat &img) {
  if (video_) {
    video_->getImage(img, frame);
    if (rze_ > 0)resize(img, img, Size(), rze_, rze_);
    stringstream lblframe;
    lblframe << "Frame " << frame;
    putText(img, lblframe.str(), Point(0, 20), CV_FONT_HERSHEY_PLAIN, 2.0, Scalar(0, 0, 255));
  }
}
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
void Sequence::drawSub(int frame, int id, Mat &img, Scalar color, string msg) {
  SubjectItem &subj = frames_[frame].subjects_[id];
  rectangle(img, subj.sw_, subj.ne_, color);
  circle(img, subj.h1_, 5, Scalar(255, 0, 255), -1, 8);
  circle(img, subj.h2_, 5, Scalar(255, 0, 255), -1, 8);
  putText(img, msg, subj.sw_, CV_FONT_HERSHEY_PLAIN, 1.0, Scalar(255, 0, 0));
}
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
void Sequence::drawObj(int frame, int id, Mat &img, Scalar color, string msg) {
  ObjectItem &obj = frames_[frame].objects_[id];
  rectangle(img, obj.sw_, obj.ne_, color);
  putText(img, msg, obj.sw_, CV_FONT_HERSHEY_PLAIN, 1.0, color);
}
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
void Sequence::drawAllSubs(int frame, Mat &img) {
  //getImage(frame, img);
  auto it = frames_.find(frame);
  for (int i = 0; i < static_cast<int> (it->second.subjects_.size()); ++i) 
    drawSub(it->second.subjects_[i], img, Scalar(100, 150, 255),"");
}
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
void Sequence::drawSub(SubjectItem &subj, Mat &img, Scalar color, string msg){
  rectangle(img, subj.sw_, subj.ne_, color);
  circle(img, subj.h1_, 5, Scalar(255, 0, 255), -1, 8);
  circle(img, subj.h2_, 5, Scalar(255, 0, 255), -1, 8);
  putText(img, msg, subj.sw_, CV_FONT_HERSHEY_PLAIN, 1.0, Scalar(255, 0, 0));
}
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
void Sequence::drawAllObjs(int frame, Mat &img) {
  //getImage(frame, img);
  auto it = frames_.find(frame);
  for (int i = 0; i < static_cast<int> (it->second.objects_.size()); ++i)
    drawObj(it->second.objects_[i], img, Scalar(0, 255, 0),"");
}
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
void Sequence::drawObj(ObjectItem &obj, Mat &img, Scalar color, string msg){
  rectangle(img, obj.sw_, obj.ne_, color);
  putText(img, msg, obj.sw_, CV_FONT_HERSHEY_PLAIN, 1.0, color);
}
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
void Sequence::write2file(string & out_file) {
  ofstream arc(out_file);
  for (auto & fr : frames_) {
    arc << "Frame " << fr.first << endl;
    for (auto & pe : fr.second.subjects_) {
      arc << "person " 
          << pe.sw_.x << " "
          << pe.sw_.y << " " 
          << pe.ne_.x - pe.sw_.x << " "
          << pe.ne_.y - pe.sw_.y << " 0.0 "
          << pe.h1_.x << " " << pe.h1_.y << " "
          << pe.h2_.x << " " << pe.h2_.y << endl;
    }
    for (auto & ob : fr.second.objects_) {
      arc << (*objs_i_)[ob.id_] << " "
          << ob.sw_.x << " "
          << ob.sw_.y << " "
          << ob.ne_.x - ob.sw_.x << " "
          << ob.ne_.y - ob.sw_.y << " 0.0\n";
    }
  }
  arc.close();
}


///############################################################################################################################################################
///############################################################################################################################################################
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
ObjectItem::ObjectItem( int id,
                        Point sw, 
                        Point ne):
id_(id){
  sw_ = sw;
  ne_ = ne;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
SubjectItem::SubjectItem( Point sw, 
                          Point ne, 
                          Point h1, 
                          Point h2):
h1_(h1),
h2_(h2){
  sw_ = sw;
  ne_ = ne;
}
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
Point FrmItem::center(){
  return Point((sw_.x + ne_.x) >> 1, (sw_.y + ne_.y) >> 1);
}
///############################################################################################################################################################
///############################################################################################################################################################
// loading operator for map
FrameSeq & FrameSeq::operator=(const FrameSeq &frm) {
  objects_  = frm.objects_;
  subjects_ = frm.subjects_;
  return *this;
}

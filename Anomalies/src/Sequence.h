#ifndef SEQUENCE_H
#define SEQUENCE_H


#include "opencv2/core/core.hpp"
#include "opencv2/imgcodecs.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"

#include "CUtil.h"
#include "fmSequence.h"


using namespace std;
using namespace cv;

struct FrmItem;
struct SubjectItem;
struct ObjectItem;
struct FrameSeq;

typedef pair<int, FrameSeq> frmPar;

//Sequence manages the sequences objects and their bbox
class Sequence {
  
  string  desc_file_,
          video_file_;
  
  short   step_;
  
  float   rze_;

  map<string, int> *objs_;
  
  map<int, string> *objs_i_;

  shared_ptr<mfVideoSequence> video_;

  void loadSequence();


private:
    

public:

  map <int, FrameSeq> frames_;

  Sequence( short                   /*step*/, 
            string &                /*desc_file*/, 
            string &                /*video_file*/,
            float                   /*rze*/,
            map<string, int> &      /*objs*/,
            map<int, string> &      /*objs_i*/);

  //frame number
  //image
  void  drawAllSubs ( int           /*frame*/,
                      Mat         & /*img*/);

  //especific subject item
  //image
  //color 
  //messag
  void  drawSub     ( SubjectItem & /*subj*/,
                      Mat         & /*img*/,
                      Scalar        /*color*/,
                      string        /*msg*/);

  //frame number
  //image
  void  drawAllObjs ( int           /*frame*/,
                      Mat         & /*img*/);

  //especific object item
  //image
  //color 
  //message
  void  drawObj     ( ObjectItem  & /*subj*/,
                      Mat         & /*img*/,
                      Scalar        /*color*/,
                      string        /*msg*/);

  //frame number
  //image
  void  getImage    ( int           /*frame*/,
                      Mat         & /*img*/);

  //frame number
  //position in vector
  //image
  //color 
  //message
  void  drawSub     ( int           /*frame*/,
                      int           /*id*/,
                      Mat         & /*img*/,
                      Scalar        /*color*/,
                      string        /*msg*/);

  //frame number
  //position in vector
  //image
  //color 
  //message
  void  drawObj     ( int           /*frame*/,
                      int           /*id*/,
                      Mat         & /*img*/,
                      Scalar        /*color*/,
                      string        /*msg*/);



  void  write2file  ( string      & /*out_file*/);

};

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

struct FrameSeq {
  vector<ObjectItem>  objects_;
  vector<SubjectItem> subjects_;
  FrameSeq  & operator = (const FrameSeq &);
};

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//Frmeitem contains (positions) subjects and objects in the scene 
struct FrmItem {
  
  Point sw_,
        ne_;
  FrmItem       (){}

  Point center  ();
};

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//for subject instance
struct SubjectItem : public FrmItem {
  Point h1_,
        h2_;
  SubjectItem ( Point /*sw*/,
                Point /*ne*/,
                Point /*h1*/,
                Point /*h2*/);
};
//for object instance
struct ObjectItem : public FrmItem {
  int   id_;
  ObjectItem (  int   /*id*/,
                Point /*sw*/,
                Point /*ne*/);
};



#endif

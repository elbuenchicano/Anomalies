#pragma once

#include "opencv2/highgui/highgui.hpp"
#include "CUtil.h"


////////////////////////////////////////////////////////////////////////////////
//Main typedefs
typedef int seqIndexType; //index type
struct SequenceBase {
  seqIndexType    pos_actual_ = 0,  //actual position
                  step_       = 1,  //step jump
                  pos_ini_    = 0,  //initial pos
                  pos_fin_    = 0,  //final pos
                  rows_       = 0,  //height
                  cols_       = 0;  //width
  //............................................................................
  //............................................................................
  SequenceBase() {}
  virtual bool getImage   ( cv::Mat &, seqIndexType ) = 0;
  virtual bool setPos     ( seqIndexType )            = 0;
  virtual void increment() { pos_actual_ += step_; }
};
////////////////////////////////////////////////////////////////////////////////
///Sequence class for videos////////////////////////////////////////////////////
struct mfVideoSequence : public SequenceBase
{
  std::string       filename_;
  cv::VideoCapture  cap_;
  //...........................................................................
  //...........................................................................
  mfVideoSequence(  std::string   file,           //filename
                    seqIndexType  step= 1,        //
                    seqIndexType  ini = 0,        //
                    seqIndexType  fin = INT_MAX){ //
    pos_ini_  = ini;
    step_     = step;
    filename_ = file;
		cap_.open(file);
		assert(cap_.isOpened());
    pos_fin_  = static_cast<seqIndexType>(
                fin <=  cap_.get(CV_CAP_PROP_FRAME_COUNT) ? //truncating the fin
                fin :   cap_.get(CV_CAP_PROP_FRAME_COUNT) );
    rows_     = static_cast<seqIndexType>(cap_.get(CV_CAP_PROP_FRAME_HEIGHT));
    cols_     = static_cast<seqIndexType>(cap_.get(CV_CAP_PROP_FRAME_WIDTH));
  }
  //___________________________________________________________________________
  ///desc: return the image frame by steped position
  ///in: returned image, relative pos, using actual pos if it is necessary
  ///out: boolean if position is correct
	bool getImage(  cv::Mat       &img,      //referenced mat
                  seqIndexType  pos = -1 ) //image at pos 
                  override{
    seqIndexType idx = pos < 0 ? pos_actual_ : pos * step_;
    if(!setPos(idx)) return false;
    cap_ >> img;
    return true;
	}
  //___________________________________________________________________________
  ///desc: set the frame number
  ///in: position 
  ///out: boolean if position is correct
  bool setPos(  seqIndexType pos) 
                override{
    if ( pos >= pos_ini_ && pos <= pos_fin_ &&
         cap_.set(CV_CAP_PROP_POS_FRAMES, pos) ) {
      pos_actual_ = pos;
      return true;
    }
    return false;
  }
};

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
///class for image sequence
class mfImageSequence : public SequenceBase{
public:
  std::string       dir_name_,
                    file_type_;
  cutil_file_cont   file_list_;
  //...........................................................................
  //...........................................................................
  mfImageSequence(  std::string   directory,  //filename
                    std::string   fileType,   //extension .jpg .bmp
                    seqIndexType  step      = 1,       //
                    seqIndexType  ini       = 0,       //
                    seqIndexType  fin       = INT_MAX, //
                    bool          dir_seek  = true,    //directory or subdirec.
                    bool          order     = true ) { //sort the filelist
                    
    pos_ini_  = ini;
    step_     = step;
    dir_name_ = directory;
    file_type_ = fileType;
    //.........................................................................
    //defined if the list includes the directory and thesubdirectories
    if (dir_seek)
      list_files(file_list_, dir_name_.c_str(), file_type_.c_str());
    else
      list_files_all(file_list_, dir_name_.c_str(), file_type_.c_str());
    std::sort(file_list_.begin(), file_list_.end(), cmpStrNum);
    pos_fin_ = static_cast<int>(
               fin <= file_list_.size() ?
               fin :  file_list_.size());
    cv::Mat tmp = cv::imread(*file_list_.begin());
    rows_ = tmp.rows;
    cols_ = tmp.cols;
  }
  //___________________________________________________________________________
  ///desc: return the image frame by steped position
  ///in: returned image, relative pos, using actual pos if it is necessary
  ///out: boolean if position is correct
  bool getImage(  cv::Mat       &img,      //referenced mat
                  seqIndexType  pos = -1) //image at pos 
    override {
    seqIndexType idx = pos < 0 ? pos_actual_ : pos * step_;
    if (!setPos(idx)) return false;
    img = cv::imread(file_list_[idx]);
    return true;
  }
  //___________________________________________________________________________
  ///desc: set the frame number
  ///in: position 
  ///out: boolean if position is correct
  bool setPos(seqIndexType pos)
    override {
    if (pos >= pos_ini_ && pos < pos_fin_) {
      pos_actual_ = pos;
      return true;
    }
    return false;
  }
};

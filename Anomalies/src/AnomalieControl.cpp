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
  typedef vector<int>* sDataType;  // square node data type, representing the frame

  typedef vector<int>* oDataType;  // object node data type, representing the index of
                          // general object vector
  typedef int eDataType;  // edge data type, representing the edge weigth

  list<Actor<sDataType, oDataType, eDataType> > active_subjects;

  
  //............................................................................
  string  seq_file;
  list<FrameItem> frame_list;
  fs_main_["featExtract_seq_file"] >> seq_file;
  
  //............................................................................
  loadFrameList(seq_file, frame_list, frame_step_, objects_);



  

  
}


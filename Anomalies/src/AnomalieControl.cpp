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
  fs_main_["main_object_file"] >> main_object_file;

  //loading functions
  loadDefinitions(main_object_file, objects_);

}
///default destructor
AnomalieControl::~AnomalieControl(){}
///
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
void AnomalieControl::featExtract() {
  typedef int sDataType;  // square node data type, representing the frame

  typedef int oDataType;  // object node data type, representing the index of
                          // general object vector
  typedef int eDataType;  // edge data type, representing the edge weigth

  vector<Actor<sDataType, oDataType, eDataType> > subjects;
  
  //............................................................................



}


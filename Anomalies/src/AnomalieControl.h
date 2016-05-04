#ifndef ANOMALIE_CONTROL_H
#define ANOMALIE_CONTROL_H
//aditional libs


#include "opencv2/core/core.hpp"
#include "opencv2/imgcodecs.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "Support.h"


//general namespaces
using namespace std;
using namespace cv;
//..............................................................................
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
///Main control class for anomalie detection
class AnomalieControl
{
  //variables...................................................................
  string            main_prop_file_;

  FileStorage       fs_main_;

  map<int, string>  objects_;




public:
  //constructors destructor.....................................................
  AnomalieControl();
  AnomalieControl(string file);
  ~AnomalieControl();

  //main functions..............................................................
  void featExtract();
  
};



#endif //_ANOMALIE_CONTROL_

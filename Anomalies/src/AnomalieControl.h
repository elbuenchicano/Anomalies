#ifndef ANOMALIE_CONTROL_H
#define ANOMALIE_CONTROL_H
//aditional libs


#include "opencv2/core/core.hpp"
#include "opencv2/imgcodecs.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "Support.h"



//..............................................................................
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
///Main control class for anomalie detection
class AnomalieControl
{
  //variables...................................................................
  string            main_prop_file_;

  FileStorage       fs_main_;

  map<string, int>  objects_;

  short             frame_step_;

  //main functions..............................................................

  void featExtract();

public:
  //constructors destructor.....................................................
  AnomalieControl();
  AnomalieControl(string file);
  ~AnomalieControl();

  void run();
  
};



#endif //_ANOMALIE_CONTROL_

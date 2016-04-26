#ifndef _ANOMALIE_CONTROL_
#define _ANOMALIE_CONTROL_
//aditional libs
#include <opencv2/core/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <iostream>
#include <string>

//general namespaces
using namespace std;
using namespace cv;
//............................................................................
///Main control class for anomalie detection
class AnomalieControl
{
  string _main_prop_file;
public:
  AnomalieControl();
  AnomalieControl(string file);
  ~AnomalieControl();
};


#endif //_ANOMALIE_CONTROL_

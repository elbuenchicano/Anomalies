#ifndef ANOMALIE_CONTROL_H
#define ANOMALIE_CONTROL_H

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

  void graphBuilding(); //recovering information from scenes given by cnns
  
  void graphLoading();

  //secondary functions.........................................................

  void graphDescription ( string, bool  );
  void show ( list<BaseDefinitions_tr::graphType> & );
  
public:
  //constructors destructor.....................................................
  AnomalieControl();
  AnomalieControl(string file);
  ~AnomalieControl();

  void run();
  
};



#endif //_ANOMALIE_CONTROL_

#ifndef ANOMALIE_CONTROL_H
#define ANOMALIE_CONTROL_H

#include "Support.h"

//..............................................................................
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
///Main control class for anomalie detection
class AnomalieControl
{
  typedef BaseDefinitions_tr::graphType graphType;
  typedef list<graphType>               graphLstT;

  //variables...................................................................
  string            main_prop_file_;

  FileStorage       fs_main_;

  map<string, int>  objects_;

  map<int, string>  objects_rev_;

  short             frame_step_;

  //main functions..............................................................

  void graphBuilding(); //recovering information from scenes given by cnns
  
  void training     ();

  //secondary functions.........................................................

  void graphDescription ( string, bool  );
  //graph to be shown
  void show             ( graphLstT /*graphs*/);
  //create dictionary 
  //receives graph list 
  void dictionaryBuild  ( graphLstT  & /*lst*/,
                          string     & /*out_file*/);
  //load graph from file
  void graphLoading     ( graphLstT & /*graphs*/,
                          string    & /*file*/ ,
                          bool        /*visual*/); 
  
public:
  //constructors destructor.....................................................
  AnomalieControl();
  AnomalieControl(string file);
  ~AnomalieControl();

  void run();
  
};



#endif //_ANOMALIE_CONTROL_

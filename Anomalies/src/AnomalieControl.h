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

  void testing      ();

  void show();

  //secondary functions.........................................................

  //graph to be shown
  
  //create dictionary 
  //receives graph list 
  void dictionaryBuild  ( graphLstT  & /*lst*/,
                          string     & /*out_file*/,
                          set<string> & /*voc*/);

  //creates distributions histograms over the vocabulary use
  void distributions    ( graphLstT   & /*lst*/,
                          string      & /*out_file*/,
                          set<string> & /*voc*/);

  
  //return image by request
  Mat show              ( int );

  //load graph from file
  
  
public:
  //constructors destructor.....................................................
  AnomalieControl();
  AnomalieControl(string file);
  ~AnomalieControl();

  void run();
  
};



#endif //_ANOMALIE_CONTROL_

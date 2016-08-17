#ifndef ANOMALIE_CONTROL_H
#define ANOMALIE_CONTROL_H

#include "Support.h"


//..............................................................................
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
///Main control class for anomalie detection
class AnomalyControl
{
  

  //variables...................................................................
  string            main_prop_file_;

  FileStorage       fs_main_;

  map<string, int>  objects_;

  map<int, string>  objects_rev_;

  short             frame_step_;

  //main functions..............................................................

  void graphBuilding(); //recovering information from scenes given by cnns
  
  void train        ();

  void training     ();

  void test         ();

  void testing1     ();

  void testing2     ();

  void show         ();

  //secondary functions.........................................................

  //graph to be shown
  
  //create dictionary 
  //receives graph list 
  
  //show the sequence
  void  show            ( graphLstT   &       /*lst*/,
                          string      &       /*video_file*/,
                          string      &       /*seq_file*/,
                          float               /*rze*/,
                          list<bool>  *       /*anomalies*/);

  //load graph from file
  void  graphBuild      ( string      &       /*seq_file*/,
                          string      &       /*video_file*/,
                          string      &       /*out_file*/,
                          float               /*rze*/,
                          double              /*distance_obj_thr*/,
                          double              /*distance_sub_thr*/,
                          int                 /*time_life*/);
  
  
public:
  //constructors destructor.....................................................
  AnomalyControl();
  AnomalyControl(string file);
  ~AnomalyControl();

  void run();
  
};



#endif //_ANOMALIE_CONTROL_

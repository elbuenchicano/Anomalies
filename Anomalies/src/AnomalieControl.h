#ifndef ANOMALIE_CONTROL_H
#define ANOMALIE_CONTROL_H

#include "Support.h"
#include "Support2.h"



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

  void test         ();

  void show         ();

  //secondary functions.........................................................

  //graph to be shown
  void show_graph       ( graphLstT   &       /*graphs*/,
                          string      &       /*video_file*/,
                          string      &       /*seq_file*/,
                          float               /*rze*/,
                          int                 /*anom_type*/);

  void show_loadedGraph ( string      &       /*setup_file*/,
                          graphLstT   &       /*graphs*/);

  //create dictionary 
  //receives graph list 
  
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

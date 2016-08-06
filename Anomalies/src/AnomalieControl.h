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

  void testing1     ();
  void testing2     ();

  void show();

  //secondary functions.........................................................

  //graph to be shown
  
  //create dictionary 
  //receives graph list 
  void dictionaryBuild  ( graphLstT   &       /*lst*/,
                          string      &       /*out_file*/,
                          set<string> &       /*voc*/);

  //creates distributions histograms over the vocabulary use
  void distributions    ( graphLstT   &       /*lst*/,
                          string      &       /*out_file*/,
                          set<string> &       /*voc*/);

  void distributionBuild( graphLstT   &       /*lst*/,
                          string      &       /*out_file*/,
                          set<int>    &       /*obsO*/,                          
                          map<string, int> &  /*voc*/);

  //creates distribution histograms over only with permutations
  Mat_<int> distribution( graphType   &       /*graph*/,
                          map<string, int> &  /*dist*/);

  //numerates the observed objects
  void listObservedObjs ( graphLstT   &       /*lst*/,
                          string      &       /*out_file*/,
                          set<int>    &       /*voc*/);

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

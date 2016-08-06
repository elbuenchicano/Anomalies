#ifndef _SUPPORT_
#define _SUPPORT_

#include "SupportClasses.h"

///_____________________________________________________________________________
///MAIN SUPPORT CONTROL FUNCTIONS

/////FEAT EXTRACT............................................................... 
////GRAPH BUILD.................................................................
//load objects names and ids
void loadDefinitions    ( string, 
                          map<string, int> & , 
                          map<int, string> &);

//load frame items  
void loadFrameItems     ( vector<string> &, 
                          list<FrameItem>::iterator &,
                          map<string, int> & );

//load frameList
void loadFrameList      ( string, 
                          list<FrameItem> &, short, 
                          map<string, int> &);

//compute the distance point to object bounding box
double  distance2object(  TrkPoint /*point*/, 
                          TrkPoint /*NortWest point*/, 
                          TrkPoint /*SouthEast point*/);

///_____________________________________________________________________________
////GRAPH LOADING 

//load described graph
void loadDescribedGraphs( string, 
                          list<BaseDefinitions_tr::graphType> &, 
                          set<int> *);

//resuming graph, clean consecutive atomic nodes
void resumingGraphs     ( list<BaseDefinitions_tr::graphType> &,
                          list<BaseDefinitions_tr::graphType> &);

//set items to string
string set2str          ( set<int> &);

//returning the graph histogram 
Mat_<int> graphHistrogram   ( BaseDefinitions_tr::graphType &, 
                              map<string, int>  &,
                              set<string> &);

//creating the map for istograms
void distributionPermutation( set<int> &, map<string, int> &);




#endif// _SUPPORT_

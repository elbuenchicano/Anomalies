#ifndef _SUPPORT_
#define _SUPPORT_

#include "SupportClasses.h"

///_____________________________________________________________________________
///MAIN SUPPORT CONTROL FUNCTIONS

/////FEAT EXTRACT............................................................... 
////GRAPH BUILD.................................................................
//load objects names and ids
void loadDefinitions( string, map<string, int> & );

//load frame items  
void loadFrameItems ( ifstream &, FrameItem &, map<string, int> & );
//load frameList

void loadFrameList  ( string, list<FrameItem> &, short, map<string, int> &);
//compute the distance point to object bounding box
double  distance2object(  TrkPoint /*point*/, 
                          TrkPoint /*NortWest point*/, 
                          TrkPoint /*SouthEast point*/);

///_____________________________________________________________________________
////GRAPH COMPARISON 
//load described graph
void loadDescribedGraphs(string, list<BaseDefinitions_tr::graphType> &);

#endif// _SUPPORT_

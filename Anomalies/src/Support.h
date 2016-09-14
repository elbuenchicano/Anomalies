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
double  distance2object ( TrkPoint      /*point*/, 
                          TrkPoint      /*NortWest point*/, 
                          TrkPoint      /*SouthEast point*/);

//split graph into small graphs
void  splitGraph        ( Observed  &         /*obs*/,
                          list<graphType> &   /*splits*/,
                          short               /*tam*/);

//compute the jaccard of two bounding box
double jaccardBbox      ( Point &, Point &, Point &, Point &);


///_____________________________________________________________________________
////GRAPH LOADING 

//load described graph
void loadDescribedGraphs( string, 
                          list<Observed> &, 
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

///_____________________________________________________________________________
////Generic load list graphs from directoy
bool loadGraphs(  string          & /*set_file*/, 
                  list<Observed>  & /*graph_list*/,
                  set<int>        & /*observedObjs*/);

///_____________________________________________________________________________
///TRAINING AND TESTING

//this function performs the extraction for first level of anomaly 
bool trainLevel1( string &, short, map<string, int> *, map<int, string> *,
                  list<Observed>  &, set<int>&, int);

//this function performs test for first level of anomaly 
bool testLevel1(  string &, short, map<string, int> *, map<int, string> *,
                  list<Observed>  &, set<int>&, int);

//this function performs the extraction for first level of anomaly 
bool trainLevel2( string &, short, map<string, int> *, map<int, string> *,
                  list<Observed>  &, set<int>&, int);

//this function performs test for second level of anomaly 
bool testLevel2(  string &, short, map<string, int> *, map<int, string> *,
                  list<Observed>  &, set<int>&, int);

//function vector
//command separated by spaces
//setting file
//frame step
//objects map  <name id>
//inverted objects map <id name>
void executeFunctionVec(  vector<pf>  &       /*vec*/, 
                          string      &       /*cmd*/,
                          string      &       /*settings_file*/,
                          short               /*frm_step*/, 
                          map<string, int> *  /*objs*/, 
                          map<int, string> *  /*objs_i*/,
                          graphLstT   &       /*graphs*/);

void dictionaryBuild   (  graphLstT   &       /*lst*/,
                          string      &       /*out_file*/,
                          set<string> &       /*voc*/);

  //creates distributions histograms over the vocabulary use
void distributions     (  graphLstT   &       /*lst*/,
                          string      &       /*out_file*/,
                          set<string> &       /*voc*/);

void distributionBuild  ( graphLstT   &       /*lst*/,
                          string      &       /*out_file*/,
                          set<int>    &       /*obsO*/,                          
                          map<string, int> &  /*voc*/);

//creates distribution histograms over only with permutations
Mat_<int> distribution  ( graphType   &       /*graph*/,
                          map<string, int> &  /*dist*/);

//numerates the observed objects
void listObservedObjs   ( graphLstT   &       /*lst*/,
                          string      &       /*out_file*/,
                          set<int>    &       /*voc*/);


///_____________________________________________________________________________
///VALIDATION

//load gt file into an ordered vector
void loadGt             ( string      &       /*file*/,
                          set<AnomalyGt> & /*anomalies*/);

//validate a graphlist(sequence)
//graph list
//gt file in correct format
//output file
//jaccard threshold 
//function pointer to algorithm to construct the anomalies for graph
void validateGraphList  ( list<Observed> &    /*graphs*/,
                          string      &       /*gt_file*/,
                          string      &       /*out_file*/,
                          pair<double, double>/*jaccard_th*/,
                          void (*vectorBuild)( //Funtion pointer
                            Observed          & /*graph*/,
                            set<AnomalyGt> & /*ans*/
                            )      
                       ); 

void computeMetrics     ( set<AnomalyGt> &    /*gt*/,
                          set<AnomalyGt> &    /*ans*/,
                          Metric_units   &    /*metrics*/);

#endif// _SUPPORT_

#ifndef SUPPORT_2_H
#define SUPPORT_2_H

#include "SupportClasses.h"
#include "Sequence.h"
#include "Support.h"
#include "Hungarian.h"
#include "RelationGraph.h"
#include <fstream>

//this function performs test for second level of anomaly 
bool testLevel3(  string &, short, map<string, int> *, map<int, string> *,
                  list<Observed>  &, set<int>&, int);

double matchNodes(Observed &, list<Observed> &, int, double);

double getDistance(Matrix<double>, int&, int, int, double&);

graphType resume(Observed &);

void AnomaliesSet(Observed&, set<AnomalyGt>&);


double degreeComparison(int, int);
double adjacenciesComparison(pair<RelationNodeObject<ActorLabel>, double> &, pair<RelationNodeObject<ActorLabel>, double> &, int &);
double neighborComparison(graphType &, graphType &,
                          _List_iterator<_List_val<_List_simple_types<RelationNodeSubject<ActorLabel, ActorLabel, double>>>>,
                          _List_iterator<_List_val<_List_simple_types<RelationNodeSubject<ActorLabel, ActorLabel, double>>>>);

#endif // !SUPPORT_2_H
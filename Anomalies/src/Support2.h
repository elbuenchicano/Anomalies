#ifndef SUPPORT_2_H
#define SUPPORT_2_H

#include "SupportClasses.h"
#include "Sequence.h"
#include "Support.h"
#include "Hungarian.h"
#include <fstream>

//this function performs test for second level of anomaly 
bool testLevel3(  string &, short, map<string, int> *, map<int, string> *,
                  list<Observed>  &, set<int>&, int);

double matchNodes(Observed &, list<Observed> &, string);

double getDistance(Matrix<double>, int&, int, int, double&);

graphType resume(Observed &);

#endif // !SUPPORT_2_H
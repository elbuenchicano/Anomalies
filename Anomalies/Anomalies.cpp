// Anomalies.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#include <iostream>
#include "src\Tracklet.h"
#include "src\RelationGraph.h"
#include <string>


using namespace cv;
using namespace std;





int main(int arg, char ** argv)
{
  
  RelationGraph<int, int, int> rg;
  rg.addSubjectNode(1);
  rg.addObjectRelation(10, 10);
  rg.addObjectRelation(10, 11);
  rg.addSubjectNode(2);
  rg.addObjectRelation(20, 21);
  rg.addObjectRelation(20, 22);


  return 0;

}


// Anomalies.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#include <iostream>
#include "src\Tracklet.h"

using namespace cv;
using namespace std;

int main(int arg, char ** argv)
{
  
  Rect2f   re(1,1,3,3);
  Tracklet trk(re);
  return 0;

}


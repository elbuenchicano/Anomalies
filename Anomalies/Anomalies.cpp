// Anomalies.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#include "src\Tracking.h"

#include <iostream>

using namespace std;

int main(int arg, char ** argv)
{
  TrackingKalman trk;
  trk.newTrack(TrkPoint(1, 1));

  
  for (int i = 2; i < 100; i += 5) {
    trk.estimate(TrkPoint(i, i));
  }
  /*cout << trk.estimate(TrkPoint(101, 101)) << endl;

  cout<<trk.estimate(TrkPoint(105, 90))<<endl;
  
  cout << trk.estimate(TrkPoint(110, 70))<<endl;

  cout<<trk.estimate(TrkPoint(21, 10))<<endl;*/

  cout << trk.predict()<<endl;
  cout << trk.predict() << endl;
  cout << trk.predict() << endl;
  cout << trk.predict() << endl;

  

  return 0;
}


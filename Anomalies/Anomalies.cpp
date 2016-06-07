
#include "stdafx.h"
#include "src\AnomalieControl.h"


int main(int arg, char ** argv)
{
/* 
  string static_file = "e:/research/anomalies/gabriel/main.yml";

  AnomalieControl AControl(arg > 1 ? argv[1] : static_file);

  AControl.run();
  /**/
  //trk_kalmanMouseExample();
  mthPoint p(4, 0);
  mthLine  l(3, -4, 0);
  cout << mth_distance_point_line(p, l);
  
  return 0;
}


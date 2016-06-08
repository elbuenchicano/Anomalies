
#include "stdafx.h"
#include "src\AnomalieControl.h"


int main(int arg, char ** argv)
{
 
  string static_file = "e:/research/anomalies/gabriel/main.yml";

  AnomalieControl AControl(arg > 1 ? argv[1] : static_file);

  AControl.run();
  /**/
 
  return 0;
}


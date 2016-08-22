
#include "stdafx.h"
#include "src\AnomalieControl.h"


int main(int arg, char ** argv)
{
 
  string static_file = "e:/research/anomalies/Matheus/graphs/scripts/test1.yml";
  //string static_file = "e:/research/anomalies/Matheus/graphs/show.yml";

  //string static_file = "e:/research/anomalies/Matheus/seq6/main.yml";

  AnomalyControl AControl(arg > 1 ? argv[1] : static_file);

  AControl.run();

  
  return 0;
}


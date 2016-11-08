
#include "stdafx.h"
#include "src\AnomalieControl.h"





int main(int arg, char ** argv)
{



 
  //string static_file = "e:/research/anomalies/Matheus/graphs/scripts/test1.yml";
  //string static_file = "e:/research/anomalies/Matheus/graphs/show.yml";

  //string static_file = "D:/Iniciacao/Anomalies/main.yml";
  //string static_file = "e:/research/anomalies/matheus/graphs/scripts/test1.yml";
  string static_file = "z:/tmp/anomalies/scripts/general/video_00050.yml";
  //string static_file = "z:/tmp/anomalies/scripts/train_scripts/train.yml";
  //string static_file = "z:/tmp/anomalies/scripts/test_scripts/test1.yml";

  AnomalyControl AControl(arg > 1 ? argv[1] : static_file);

  AControl.run();

  /**/
  return 0;
}



#include "stdafx.h"
#include "src\AnomalieControl.h"


int main(int arg, char ** argv)
{
  
  string static_file = "e:/research/anomalies/main.yml";

  AnomalieControl AControl(arg > 1 ? argv[1] : static_file);

  return 0;
}


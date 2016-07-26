
#include "stdafx.h"
#include "src\AnomalieControl.h"


int main(int arg, char ** argv)
{
 
  string static_file = "e:/research/anomalies/Matheus/main.yml";

  //string static_file = "e:/research/anomalies/Matheus/seq2/main.yml";

  AnomalieControl AControl(arg > 1 ? argv[1] : static_file);

  AControl.run();
  /**/
 /*
  vector<string > vec{ "aa" , "ba","ca" ,"da" ,"ea" ,"fa"  };
  int pos;
  cutil_bin_search<string>(vec, "fa", pos);
  
  cutil_cont2os<vector<string>>(cout, vec, " - \n");
  */
  return 0;
}


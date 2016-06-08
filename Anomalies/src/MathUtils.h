#ifndef MATHUTILS_H
#define MATHUTILS_H

#include <cmath>


//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
//Cutil is a utilitaries c++ functions for algebraic solutions................
//----------------------------------------------------------------------------


//-----------------Definciones-----------------------------------------------
//----------------------------------------------------------------------------

#define _PI 3.14159265358979323846


struct  mthPoint;
struct  mthLine;


//-----------------variables--------------------------------------------------
//----------------------------------------------------------------------------

///////////////////////////FUNCIONES//////////////////////////////////////////

//distance point to line 
double mth_distance_point_line ( mthPoint &, mthLine &);

///////////////////////////CLASSES-STRUCTS////////////////////////////////////
//basic structure of point
struct mthPoint { 
  //variables
  double  x_, 
          y_; 
  
  //constructors
  mthPoint ( double, double );
  mthPoint ( mthPoint & );
  
  //functions
  mthPoint &operator = ( mthPoint & );
};

//............................................................................
//basic structure of line equation 
struct mthLine  {
  //variables
  //ax + by = c =0
  double  a_, 
          b_, 
          c_;
  
  //constructors
  mthLine ( double, double, double );
  mthLine ( mthPoint, mthPoint );

  //functions
  mthLine &operator = (mthLine & );
  
};


#endif // !MATHCUTILS_H
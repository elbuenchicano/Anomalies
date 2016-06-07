#include "MathUtils.h"


////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
double mth_distance_point_line(mthPoint &point, mthLine &line){
  auto num = abs ( point.x_ * line.a_ + point.y_ * line.b_ + line.c_ );
  auto den = sqrt( line.a_ * line.a_ + line.b_ * line.b_ );
  return num / den;
}


////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////MTHPOINT
//constructors
mthPoint::mthPoint(double x, double y):
x_(x),
y_(y){}

mthPoint::mthPoint(mthPoint &p){
  x_ = p.x_;
  y_ = p.y_;
}

mthPoint &mthPoint::operator=(mthPoint &p){
  x_ = p.x_;
  y_ = p.y_;
  return *this;
}


////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////MTHLINE
//constructors
mthLine::mthLine(double a, double b, double c):
a_(a),
b_(b),
c_(c){}

//slope equation constructor using two points
mthLine::mthLine(mthPoint p1, mthPoint p2){
  a_ = ( p2.y_ - p1.y_ );
  b_ = -( p2.x_ - p2.x_ );
  c_ = a_ * p1.x_ + b_ * p1.y_;
}

mthLine &mthLine::operator=(mthLine &l) {
  a_ = l.a_;
  b_ = l.b_;
  c_ = l.c_;
  return *this;
}




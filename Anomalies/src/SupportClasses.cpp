#include "SupportClasses.h"
//##############################################################################
//ACTOR LABEL
//constructors
ActorLabel::ActorLabel(int id, int lst_idx) {
  id_       = id;
  list_idx_ = lst_idx;
}

//oveload operators
ostream & operator<<(ostream & os, ActorLabel & ac){
  os << ac.id_ << " " << ac.list_idx_;
  return os;
}

ActorLabel & ActorLabel::operator=(ActorLabel & ac){
  id_       = ac.id_;
  list_idx_ = ac.list_idx_;
  return *this;
}

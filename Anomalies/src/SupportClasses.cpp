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
  anomaly_  = ac.anomaly_;
  return *this;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//##############################################################################
//ANOMALY GT
AnomalyGt::AnomalyGt(int id, int ini, int fin, int type, string & desc):
id_(id),
ini_(ini),
fin_(fin),
type_(type),
desc_(desc){
}
////////////////////////////////////////////////////////////////////////////////
bool operator==(const AnomalyGt &an, const AnomalyGt &an2) {
  return (an2.ini_ == an.ini_);
}

////////////////////////////////////////////////////////////////////////////////
bool operator<(const AnomalyGt &an, const AnomalyGt &an2) {
  return (an.ini_ < an2.ini_);
}

////////////////////////////////////////////////////////////////////////////////
AnomalyGt AnomalyGt::operator=(const AnomalyGt &an){
  ini_  = an.ini_;
  fin_  = an.fin_;
  id_   = an.id_;
  type_ = an.type_;
  desc_ = an.desc_;
  return *this;
}

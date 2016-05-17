#include "Tracklet.h"
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//constructors and destructors

Tracklet::Tracklet(){
  trk_    = new TrackingKalman;
}

Tracklet::~Tracklet(){
  if (trk_) delete trk_;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//main functions 

///feed function feeds the tracklet with new observation
///in: region of new observation
void Tracklet::find_next(TrlRegion & reg){
  region_ = reg;
  trk_->predict();
  region_ = trk_->estimate(region_);
}

///initiate the tracklet
///in: first observation 
void Tracklet::initiate(TrlRegion & reg) {
  region_ = reg;
  TrkPoint centroid(region_.x, region_.y );
  trk_->newTrack(centroid);
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
#include "PhotonHit.h"

#include <math.h>

#ifndef PHOTON_EQ_DST
#define PHOTON_EQ_DST 0.001
#endif

#ifndef PHOTON_EQ_ANG
#define PHOTON_EQ_ANG (M_PI/100.0)
#endif

PhotonHit::PhotonHit()
{
    *this=PhotonHit(Point::null,Vector::null,Color::Black);
}

PhotonHit::PhotonHit(const Point& p,const Vector& v,const Color& c)
{
    this->p=p,this->v=v,this->c=c;
}

PhotonHit::~PhotonHit() {}

bool PhotonHit::operator==(const PhotonHit& that)const
{
    return this->getPoint().dist(that.getPoint())<PHOTON_EQ_DST
           &&fabs(this->getVector().angle(that.getVector()))<PHOTON_EQ_ANG;
}

void PhotonHit::addColor(const Color& col)
{
    this->c+=col;
}

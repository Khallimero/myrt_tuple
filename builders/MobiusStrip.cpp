#include "MobiusStrip.h"

#include <math.h>

MobiusStrip::MobiusStrip(double r,int w)
{
    this->r=r,this->w=w;
}

MobiusStrip::~MobiusStrip() {}

Point MobiusStrip::getPoint(const Tuple<double,2>& t)const
{
    const double u=t.get(IPair::first);
    const double v=r*((t.get(IPair::second)/M_PI)-1.0)/2.0;

    double x=(r+v*cos(w*u/2.0))*cos(u);
    double y=v*sin(w*u/2.0);
    double z=(r+v*cos(w*u/2.0))*sin(u);

    return Point(x,y,z);
}

#include "ParametricTore.h"

#include <math.h>

ParametricTore::ParametricTore(double r)
{
    this->r=r;
}

ParametricTore::~ParametricTore() {}

Point ParametricTore::getPoint(const Tuple<double,2>& t)const
{
    const double u=t.get(IPair::first);
    const double v=t.get(IPair::second);

    double x=cos(u)-(this->r*cos(u)*cos(v));
    double y=-this->r*sin(v);
    double z=sin(u)-(this->r*sin(u)*cos(v));

    return Point(x,y,z);
}

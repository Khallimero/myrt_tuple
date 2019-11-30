#include "BoyApery.h"

#include <math.h>

BoyApery::BoyApery(double a,double b)
{
    this->a=a,this->b=b;
}

BoyApery::~BoyApery() {}

Point BoyApery::getPoint(const Tuple<double,2>& t)const
{
    const double u=t.get(IPair::first)/2.0;
    const double v=t.get(IPair::second)/2.0;

    double x=a*(cos(u)*cos(2.0*v)+b*sin(u)*cos(v))*cos(u);
    double z=a*(cos(u)*sin(2.0*v)-b*sin(u)*sin(v))*cos(u);
    double y=b*SQ(cos(u));

    return Point(x,y,z)/(b-sin(2.0*u)*sin(3.0*v));
}

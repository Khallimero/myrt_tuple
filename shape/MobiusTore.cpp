#include "MobiusTore.h"

#include <math.h>

MobiusTore::MobiusTore(double a,double m,double n)
{
    this->a=a,this->m=m,this->n=n;
}

MobiusTore::~MobiusTore() {}

Point MobiusTore::getPoint(const Tuple<double,2>& t)const
{
    const double u=t.get(IPair::first);
    const double v=t.get(IPair::second);

    double x=(a+cos(n*u)*sin(v)-sin(n*u)*sin(2.0*v))*cos(m*u);
    double y=sin(n*u)*sin(v)+cos(n*u)*sin(2.0*v);
    double z=(a+cos(n*u)*sin(v)-sin(n*u)*sin(2.0*v))*sin(m*u);

    return Point(x,y,z);
}

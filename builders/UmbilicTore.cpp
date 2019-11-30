#include "UmbilicTore.h"

#include <math.h>

UmbilicTore::UmbilicTore() {}

UmbilicTore::~UmbilicTore() {}

Point UmbilicTore::getPoint(const Tuple<double,2>& t)const
{
    const double u=t.get(IPair::first);
    const double v=t.get(IPair::second);
    const double r=7.0+cos(u/3.0-2.0*v)+2.0*cos(u/3.0+v);

    double x=r*sin(u);
    double y=sin(u/3.0-2.0*v)+2*sin(u/3.0+v);
    double z=r*cos(u);

    return Point(x,y,z);
}

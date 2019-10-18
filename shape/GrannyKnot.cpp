#include "GrannyKnot.h"

#include <math.h>

GrannyKnot::GrannyKnot() {}

GrannyKnot::~GrannyKnot() {}

Point GrannyKnot::getPoint(const Tuple<double,1>& t)const
{
    const double a=get(t);
    double x=0.5*cos(a)+0.25*cos(-a)-0.5*cos(-3.0*a);
    double y=0.25*sin(4.0*a)+0.185*sin(2.0*a);
    double z=0.5*sin(a)+0.25*sin(-a)-0.5*sin(-3.0*a);

    return Point(x,y,z);
}

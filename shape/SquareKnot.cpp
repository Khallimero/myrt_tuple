#include "SquareKnot.h"

#include <math.h>

SquareKnot::SquareKnot() {}

SquareKnot::~SquareKnot() {}

Point SquareKnot::getPoint(const Tuple<double,1>& t)const
{
    const double a=get(t);
    double x=0.5*cos(a)+0.25*cos(-a)-0.5*cos(-3.0*a);
    double y=0.25*sin(5.0*a);
    double z=0.5*sin(a)+0.25*sin(-a)-0.5*sin(-3.0*a);

    return Point(x,y,z);
}

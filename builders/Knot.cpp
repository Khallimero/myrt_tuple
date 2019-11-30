#include "Knot.h"

#include <math.h>

Knot::Knot(int p,int q,
           double m,double n,
           double h,double t)
{
    this->p=p,this->q=q;
    this->m=m,this->n=n;
    this->h=h,this->t=t;
}

Knot::~Knot() {}

Point Knot::getPoint(const Tuple<double,1>& s)const
{
    const double a=get(s);
    double x=m*cos((double)p*a)+n*cos((double)q*a);
    double y=h*sin(t*a);
    double z=m*sin((double)p*a)+n*sin((double)q*a);

    return Point(x,y,z);
}

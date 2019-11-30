#include "TorusKnot.h"
#include "Tore.h"

#include <math.h>

TorusKnot::TorusKnot(int p,int q,double rt)
{
    this->p=p,this->q=q;
    this->rt=rt;
}

TorusKnot::~TorusKnot() {}

Point TorusKnot::getPoint(const Tuple<double,1>& t)const
{
    const double a=get(t);

    double r=cos((double)q*a)+rt;
    double x=r*cos((double)p*a);
    double y=sin((double)q*a);
    double z=r*sin((double)p*a);

    return Point(x,y,z);
}

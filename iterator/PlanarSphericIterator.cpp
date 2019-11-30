#include "PlanarSphericIterator.h"

#include <math.h>

PlanarSphericIterator::PlanarSphericIterator(int n,const Point& p,double r)
    :SphericIterator(n,p,r)
{
    this->ns=sqrt(M_PI*(double)this->getSteps())/2.0;
    reset();
}

PlanarSphericIterator::~PlanarSphericIterator() {}

void PlanarSphericIterator::reset()
{
    this->id=-1;
}

bool PlanarSphericIterator::next()
{
    return (id++)<this->getSteps();
}

Point PlanarSphericIterator::current()const
{
    double a=asin(2.0*((double)this->id/(double)this->getSteps())-1.0)+(M_PI/2.0);
    double x=this->r*cos(a*2.0*this->ns)*sin(a);
    double y=this->r*sin(a*2.0*this->ns)*sin(a);
    double z=r*cos(a);

    return this->getPoint()+Point(x,y,z);
}

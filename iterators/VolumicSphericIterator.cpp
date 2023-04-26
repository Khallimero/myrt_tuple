#include "VolumicSphericIterator.h"

VolumicSphericIterator::VolumicSphericIterator(int n,const Point& p,double r)
    :SphericIterator(n,p,r),it(Iterator<double>(n,(r*2.0)/(double)(n-n%2)))
{}

VolumicSphericIterator::~VolumicSphericIterator() {}

void VolumicSphericIterator::reset()
{
    it.reset();
}

bool VolumicSphericIterator::next()
{
    while(it.next())
        if(this->getPoint().dist(current())<this->getRadius()+EPSILON)
            return true;
    return false;
}

Point VolumicSphericIterator::current()const
{
    return this->getPoint()+it.getTuple();
}

#include "SphericIterator.h"

SphericIterator::SphericIterator(int n,const Point& p,double r)
{
    this->steps=n,this->p=p,this->r=r;
}

SphericIterator::~SphericIterator() {}

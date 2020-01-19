#include "SphericIterator.h"

SphericIterator::SphericIterator(int n,const Point& p,double r)
    :steps(n),p(p),r(r)
{}

SphericIterator::~SphericIterator() {}

#include "RoundedIterator.h"

RoundedIterator::RoundedIterator():NestedIterator<double,2>()
{
    this->r=0;
}

RoundedIterator::RoundedIterator(const Iterator<double>& it)
    :NestedIterator<double,2>(it,2)
{
    this->r=it.maxVal();
}

RoundedIterator::~RoundedIterator() {}

bool RoundedIterator::next()
{
    bool hasNext=true;
    do
    {
        hasNext=NestedIterator<double,2>::_next();
    }
    while(hasNext&&(getRadius()>this->r));
    if(hasNext)nb++;
    return hasNext;
}

double RoundedIterator::getRadius()const
{
    double d=0;
    for(int i=0; i<nbIt; i++)
        d+=SQ(getElement(i).getVal());
    return sqrt(d);
}

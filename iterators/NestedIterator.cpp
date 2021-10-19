#include "NestedIterator.h"

#include "Vector.h"

template class NestedIterator<double,2>;
template class NestedIterator<int,TREBLE_SIZE>;

INestedIterator::INestedIterator()
{
    this->nbIt=0,this->nb=0;
}

INestedIterator::~INestedIterator() {}

bool INestedIterator::hasNext()const
{
    for(int i=0; i<nbIt; i++)
        if(_hasNext(i))return true;
    return false;
}

bool INestedIterator::next()
{
    bool nxt=_next();
    if(nxt)nb++;
    return nxt;
}

bool INestedIterator::_next()
{
    for(int i=nbIt; i>0; i--)
        if(_hasNext(i-1))
        {
            reset(i-1,true);
            return true;
        }
    return false;
}

int INestedIterator::getMaxSteps()const
{
    int n=1;
    for(int i=0; i<nbIt; i++)
        n*=_getSteps(i);
    return n;
}

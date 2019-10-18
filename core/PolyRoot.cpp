#include "PolyRoot.h"

PolyRoot::PolyRoot(bool collect)
{
    this->firstRoot=-1;
    this->collect=collect;
}

PolyRoot::~PolyRoot() {}

void PolyRoot::addRoot(double r)
{
    if(collect)_add(r);

    if(firstRoot<0||(r>0&&r<firstRoot))
        firstRoot=r;
}

#pragma once

#include "SphericIterator.h"
#include "NestedIterator.h"

class VolumicSphericIterator:public SphericIterator
{
public:
    VolumicSphericIterator(int n,const Point& p,double r);
    virtual ~VolumicSphericIterator();

public:
    virtual void reset();
    virtual bool next();
    virtual Point current()const;

protected:
    NestedIterator<double,3> it;
};

#pragma once

#include "SphericIterator.h"

class PlanarSphericIterator:public SphericIterator
{
public:
    PlanarSphericIterator(int n,const Point& p,double r);
    virtual ~PlanarSphericIterator();

public:
    virtual void reset();
    virtual bool next();
    virtual Point current()const;

protected:
    int id;
    double ns;
};

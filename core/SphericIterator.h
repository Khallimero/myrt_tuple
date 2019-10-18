#pragma once

#include "Point.h"

class SphericIterator
{
public:
    SphericIterator(int n,const Point& p,double r);
    virtual ~SphericIterator();

public:
    virtual void reset()=0;
    virtual bool next()=0;
    virtual Point current()const=0;

public:
    int getSteps()const
    {
        return steps;
    }
    const Point& getPoint()const
    {
        return p;
    }
    double getRadius()const
    {
        return r;
    }

protected:
    int steps;
    Point p;
    double r;
};

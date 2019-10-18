#pragma once

#include "Vector.h"
#include "Point.h"

class BumpMap
{
public:
    BumpMap(double f=1.0);
    virtual ~BumpMap();

public:
    virtual Vector getVector(const Point&)const=0;

protected:
    double fct;
};

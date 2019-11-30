#pragma once

#include "ParametricPrimitive.h"

class MobiusStrip:public ParametricPrimitive<2>
{
public:
    MobiusStrip(double r,int w=1);
    virtual ~MobiusStrip();

public:
    virtual Point getPoint(const Tuple<double,2>& t)const;

protected:
    double r;
    int w;
};

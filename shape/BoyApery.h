#pragma once

#include "ParametricPrimitive.h"

class BoyApery:public ParametricPrimitive<2>
{
public:
    BoyApery(double a,double n);
    virtual ~BoyApery();

public:
    virtual Point getPoint(const Tuple<double,2>& t)const;

protected:
    double a,b;
};

#pragma once

#include "ParametricPrimitive.h"

class ParametricTore:public ParametricPrimitive<2>
{
public:
    ParametricTore(double r);
    virtual ~ParametricTore();

public:
    virtual Point getPoint(const Tuple<double,2>& t)const;

protected:
    double r;
};

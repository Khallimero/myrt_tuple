#pragma once

#include "ParametricPrimitive.h"

class TorusKnot:public ParametricPrimitive<1>
{
public:
    TorusKnot(int p,int q,double rt);
    virtual ~TorusKnot();

public:
    virtual Point getPoint(const Tuple<double,1>& t)const;

protected:
    int p,q;
    double rt;
};

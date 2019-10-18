#pragma once

#include "ParametricPrimitive.h"

class MobiusTore:public ParametricPrimitive<2>
{
public:
    MobiusTore(double a,double m,double n);
    virtual ~MobiusTore();

public:
    virtual Point getPoint(const Tuple<double,2>& t)const;

protected:
    double a,m,n;
};

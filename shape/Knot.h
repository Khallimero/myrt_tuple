#pragma once

#include "ParametricPrimitive.h"

class Knot:public ParametricPrimitive<1>
{
public:
    Knot(int p,int q,
         double m,double n,
         double h,double t);
    virtual ~Knot();

public:
    virtual Point getPoint(const Tuple<double,1>& s)const;

protected:
    int p,q;
    double m,n;
    double h,t;
};

#pragma once

#include "IsoSurface.h"

class OrthoCircles:public IsoSurface
{
public:
    OrthoCircles(double c1,double c2,
                 double size,
                 const Mark& mk=Mark::Ref);
    virtual ~OrthoCircles();

protected:
    virtual double getValue(const Point& p)const;
    virtual Vector getNormal(const Point& p)const;

protected:
    double c1,c2;
};

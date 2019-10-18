#pragma once

#include "IsoSurface.h"

class Gyroid:public IsoSurface
{
public:
    Gyroid(double c,
           double size,
           const Shape* box,
           const Mark& mk=Mark::Ref);
    virtual ~Gyroid();

protected:
    virtual double getValue(const Point& p)const;

protected:
    double c;
};

#pragma once

#include "IsoSurface.h"

class CustomIsoSurface:public IsoSurface
{
public:
    CustomIsoSurface(double (*fct)(const Point& p),
                     double size,
                     const Shape* box,
                     const Mark& mk=Mark::Ref);
    virtual ~CustomIsoSurface();

protected:
    virtual double getValue(const Point& p)const;

protected:
    double (*fct)(const Point& p);
};

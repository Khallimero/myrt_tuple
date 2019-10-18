#pragma once

#include "IsoSurface.h"

class DecoCube:public IsoSurface
{
public:
    DecoCube(double c1,double c2,double size,const Mark& mk=Mark::Ref);
    virtual ~DecoCube();

protected:
    Point _getPoint(const Point& p)const;

protected:
    virtual double getValue(const Point& p)const;
    virtual Vector getNormal(const Point& p)const;

protected:
    double c1,c2;
};

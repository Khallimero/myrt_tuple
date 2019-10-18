#pragma once

#include "IsoSurface.h"

class Scherk:public IsoSurface
{
public:
    Scherk(double r,double size,const Mark& mk=Mark::Ref);
    virtual ~Scherk();

protected:
    virtual double getValue(const Point& p)const;
};

#pragma once

#include "IsoSurface.h"

class Kluchikov:public IsoSurface
{
public:
    Kluchikov(double size,const Mark& mk=Mark::Ref);
    virtual ~Kluchikov();

protected:
    virtual double getValue(const Point& p)const;
};

#pragma once

#include "IsoSurface.h"

class DecoSphere:public IsoSurface
{
public:
    DecoSphere(double size,const Mark& mk=Mark::Ref);
    virtual ~DecoSphere();

protected:
    virtual double getValue(const Point& p)const;
};

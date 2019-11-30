#pragma once

#include "IsoSurface.h"

class DecicStar:public IsoSurface
{
public:
    DecicStar(double size,const Mark& mk=Mark::Ref);
    virtual ~DecicStar();

protected:
    virtual double getValue(const Point& p)const;
};

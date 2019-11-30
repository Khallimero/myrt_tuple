#pragma once

#include "IsoSurface.h"

class FractalShape:public IsoSurface
{
public:
    FractalShape(int iter,double size,
                 SmartPointer<const Shape> box,const Mark& mk=Mark::Ref);
    virtual ~FractalShape();

protected:
    virtual Vector getNormal(const Point& p)const;

protected:
    int iter;
    double nrmFct;
};

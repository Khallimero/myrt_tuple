#pragma once

#include "PolyShape.h"

#define HYPERTRIANGLE_POLY_ORDER 4

class HyperTriangle:public PolyShape<HYPERTRIANGLE_POLY_ORDER>
{
public:
    HyperTriangle(double a,double b,double k,
                  double size,
                  const Mark& mk=Mark::Ref);
    virtual ~HyperTriangle();

protected:
    virtual Hit _getHit(const Ray& r)const;
    virtual TPoly<HYPERTRIANGLE_POLY_ORDER> _getPoly(const Point& p,const Vector& v)const;

protected:
    double a,b,k;
};

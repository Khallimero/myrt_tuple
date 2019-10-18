#pragma once

#include "PolyShape.h"

#define HYPERCUBE_POLY_ORDER 4

class HyperCube:public PolyShape<HYPERCUBE_POLY_ORDER>
{
public:
    HyperCube(double fct,double size,
              const Mark& mk=Mark::Ref);
    virtual ~HyperCube();

protected:
    virtual Hit _getHit(const Ray& r)const;
    virtual TPoly<HYPERCUBE_POLY_ORDER> _getPoly(const Point& p,const Vector& v)const;

protected:
    double fct;
};

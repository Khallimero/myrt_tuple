#pragma once

#include "PolyShape.h"
#include "Sphere.h"

#define TORE_POLY_ORDER 4

class Tore:public PolyShape<TORE_POLY_ORDER>
{
public:
    Tore(double r1, double r2, const Mark& mk = Mark::Ref);
    virtual ~Tore();

public:
    virtual Point getRelativePoint(const Hit& h,Mapping mp=Volumic)const;
    virtual bool isInside(const Point& p,double e=0.0)const;
    virtual bool intersect(const Ray& r)const;

protected:
    virtual Hit _getHit(const Ray& r)const;
    virtual TPoly<TORE_POLY_ORDER> _getPoly(const Point& p,const Vector& v)const;

protected:
    double r1,r2;
    Sphere sBox;

protected:
    static const Vector Unit;
};

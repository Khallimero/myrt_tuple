#pragma once

#include "PlaneShape.h"

class Plan:public PlaneShape
{
public:
    Plan(double p,const Point& o,const Mark& mk=Mark::Ref);
    Plan(const Vector& v1,const Vector& v2,const Mark& mk);
    Plan(const Vector& n,const Mark& mk);
    virtual ~Plan();

public:
    virtual bool intersect(const Ray& r)const;

protected:
    void init(const Mark& mk);
    virtual Hit _getHit(const Ray& r)const;
    virtual double _intersectCoeff(const Ray& r)const;
    virtual Vector getNormal()const;

protected:
    double p;
};

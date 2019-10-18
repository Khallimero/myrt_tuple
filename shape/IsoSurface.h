#pragma once

#include "Shape.h"
#include "SmartPointer.h"

#ifndef ISO_STEP
#define ISO_STEP (0.01)
#endif

#ifndef ISO_THRESHOLD
#define ISO_THRESHOLD (0.00000001)
#endif

class IsoSurface:public Shape
{
public:
    IsoSurface(double size,SmartPointer<const Shape> box,const Mark& mk=Mark::Ref);
    virtual ~IsoSurface();

public:
    virtual bool isInside(const Point& i,double e=0.0)const;

protected:
    virtual Hit _getHit(const Ray& r)const;

protected:
    virtual double getValue(const Point& p)const=0;
    virtual Vector getNormal(const Point& p)const;

protected:
    double size;
    SmartPointer<const Shape> box;
};

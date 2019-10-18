#pragma once

#include "PointLight.h"

class SphericLight:public PointLight
{
public:
    SphericLight(double r,const Point& p,const Color& c,double gl=0);
    virtual ~SphericLight();

public:
    virtual NestedIterator<double,2>* getIterator(int ph)const;
    virtual Ray getRay(const Shape* s,const Point& p,const Point& o,const NestedIterator<double,2>* it)const;
    virtual const Sphere* getBox()const
    {
        return box;
    }

protected:
    double r;
    const Sphere* box;
};

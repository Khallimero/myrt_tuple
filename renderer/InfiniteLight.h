#pragma once

#include "Light.h"

class InfiniteLight:public Light
{
public:
    InfiniteLight(const Vector& v,const Color& c);
    virtual ~InfiniteLight();

public:
    virtual Vector getVectorTo(const Point& p)const;
    virtual Point getOrig(const Shape* s,const Point& p)const;
    virtual Ray getRay(const Shape*s,const Point& p,const Point& o,const NestedIterator<double,2>* it=NULL)const;
    virtual double dist(const Point& p)const;

protected:
    Vector v;
};

#pragma once

#include "Light.h"

class PointLight:public Light
{
public:
    PointLight(const Point& p,const Color& c,double gl=0);
    virtual ~PointLight();

public:
    virtual Vector getVectorTo(const Point& p)const;
    virtual Point getOrig(const Shape* s,const Point& p)const;
    virtual Ray getRay(const Shape* s,const Point& p,const Point& o,const NestedIterator<double,2>* it=NULL)const;
    virtual double dist(const Point& p)const;

protected:
    Point p;
};

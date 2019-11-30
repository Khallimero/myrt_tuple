#pragma once

#include "CSGShape.h"

class InterShape:public CSGShape
{
public:
    InterShape(SmartPointer<const Shape> s1,SmartPointer<const Shape> s2,bool merge=true);
    virtual ~InterShape();

public:
    virtual bool isInside(const Point& p,double e=0.0)const;

protected:
    virtual Hit __getHit(const Ray& r)const;
};

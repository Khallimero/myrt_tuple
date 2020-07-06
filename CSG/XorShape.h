#pragma once

#include "CSGShape.h"

class XorShape:public CSGShape
{
public:
    XorShape(SmartPointer<const Shape> s1,SmartPointer<const Shape> s2,bool merge=true);
    virtual ~XorShape();

public:
    virtual bool isInside(const Point& p,double e=0.0)const;

protected:
    virtual Hit __getHit(const Ray& r)const;
};

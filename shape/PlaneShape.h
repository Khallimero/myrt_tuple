#pragma once

#include "Shape.h"
#include "Point.h"

class PlaneShape:public Shape
{
protected:
    PlaneShape(const Mark& mk=Mark::Ref);

public:
    virtual ~PlaneShape();

public:
    virtual Point getRelativePoint(const Hit& h,Mapping mp=Undefined)const;
    virtual bool isInside(const Point& h,double =0.0)const;

public:
    const Point& getOrig()const
    {
        return orig;
    }

protected:
    virtual Vector getNormal()const=0;

protected:
    Point orig;
};

#pragma once

#include "Shape.h"
#include "ShapeCollection.h"

class ShapeCollectionShape:public Shape
{
public:
    ShapeCollectionShape(const Mark& mk=Mark::Ref);
    virtual ~ShapeCollectionShape();

public:
    virtual bool isInside(const Point& p,double e=0.0)const;
    virtual bool intersect(const Ray& r)const;
    virtual Point getRelativePoint(const Hit& h,Mapping mp=Undefined)const;

protected:
    virtual Hit _getHit(const Ray& r)const;

public:
    ShapeCollection* getShapes()
    {
        return &shapes;
    }

protected:
    ShapeCollection shapes;
};

#pragma once

#include "Collection.h"
#include "Shape.h"
#include "SmartPointer.h"

class ShapeCollection:public Collection< SmartPointer<Shape> >
{
public:
    ShapeCollection(SmartPointer<Shape> b=SmartPointer<Shape>(NULL));
    virtual ~ShapeCollection();

public:
    virtual Hit getHit(const Ray& r)const;
    virtual bool intersect(const Ray& r)const;
    virtual bool isInside(const Point& p,double e=0.0)const;

public:
    const Shape* getBox()const
    {
        return box.getPointer();
    }
    void setBox(SmartPointer<Shape> b)
    {
        this->box=b;
    }

protected:
    SmartPointer<Shape> box;
};

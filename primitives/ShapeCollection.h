#pragma once

#include "Collection.h"
#include "Shape.h"
#include "SmartPointer.h"

class ShapeCollection:public ObjCollection< SmartPointer<Shape> >
{
public:
    ShapeCollection(SmartPointer<Shape> b=SmartPointer<Shape>(NULL));
    virtual ~ShapeCollection();

public:
    virtual Hit getHit(const Ray& r)const;
    ObjCollection<Hit> getHit(const ObjCollection<Ray>& r)const;
    ObjCollection<Hit> getIntersect(const ObjCollection<Ray>& r)const;
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
    ObjCollection<Hit> _getHit(const ObjCollection<Ray>& r,ObjCollection<Hit> (Shape::*fct)(const ObjCollection<Ray>&)const)const;

protected:
    SmartPointer<Shape> box;
};

#pragma once

#include "Shape.h"
#include "Sphere.h"
#include "ShapeCollection.h"
#include "SmartPointer.h"

class BoxedCollectionShape:public Shape
{
public:
    BoxedCollectionShape(double r,const Mark& mk=Mark::Ref);
    virtual ~BoxedCollectionShape();

public:
    void addSphere(SmartPointer<Sphere> s);

public:
    virtual bool isInside(const Point& p,double e=0.0)const;
    Collection<ShapeCollection*>& getShapes()
    {
        return shapes;
    }
    void trim();

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
    virtual Hit _getHit(const Ray& r)const;

protected:
    double radius;
    SmartPointer<Shape> box;
    Collection<ShapeCollection*> shapes;
};

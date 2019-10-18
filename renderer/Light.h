#pragma once

#include "Color.h"
#include "Point.h"
#include "Vector.h"
#include "Ray.h"
#include "Shape.h"
#include "Sphere.h"
#include "NestedIterator.h"

class Light
{
public:
    Light(const Color& c,double gl=0);
    virtual ~Light();

public:
    double getGlare()const
    {
        return glare;
    }

public:
    virtual NestedIterator<double,2>* getIterator(int ph)const;
    virtual Point getOrig(const Shape* s,const Point& p)const=0;
    virtual Vector getVectorTo(const Point& p)const=0;
    virtual Ray getRay(const Shape* s,const Point& p,const Point& o,const NestedIterator<double,2>* it=NULL)const=0;

public:
    virtual double dist(const Point& p)const=0;
    const Color& getColor()const
    {
        return c;
    }
    virtual const Sphere* getBox()const
    {
        return NULL;
    }

protected:
    Color c;
    double glare;
};

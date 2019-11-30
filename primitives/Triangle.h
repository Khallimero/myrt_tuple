#pragma once

#include "PlaneShape.h"
#include "Vector.h"

class Triangle:public PlaneShape
{
public:
    Triangle(const Vector& v1,const Vector& v2,
             const Mark& mk=Mark::Ref);
    Triangle(const Point& p1,const Point& p2,const Point& p3);
    virtual ~Triangle();

protected:
    virtual Hit _getHit(const Ray& r)const;

public:
    static Hit getTriangleHit(const Ray& r,const Shape* s,const Point* p);
    static Hit getTriangleHit(const Ray& r,const Shape* s,const Point& o,const Vector& v1,const Vector& v2);
    static Vector getTriangleNormal(const Point* p);
    static Vector getTriangleNormal(const Vector& v1,const Vector& v2);

public:
    virtual Vector getNormal()const;
    virtual double getArea()const;

protected:
    Vector v1,v2;
};

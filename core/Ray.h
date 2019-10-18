#pragma once

#include "Vector.h"
#include "Point.h"

class Ray
{
public:
    Ray();
    Ray(const Point& p,const Vector& v);
    Ray(const Point& p1,const Point& p2);
    virtual ~Ray();

public:
    bool operator==(const Ray& that)const;
    double dist(const Point& t)const;
    Point proj(const Point& t)const;
    virtual bool isNull()const;

public:
    const Point& getPoint()const
    {
        return p;
    }
    void setPoint(const Point& p)
    {
        this->p=p;
    }
    const Vector& getVector()const
    {
        return v;
    }
    void setVector(const Vector& v)
    {
        this->v=v;
    }

public:
    static const Ray null;

protected:
    Point p;
    Vector v;
};

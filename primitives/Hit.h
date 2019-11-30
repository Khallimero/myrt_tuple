#pragma once

#include "Point.h"
#include "Vector.h"
#include "Ray.h"

class Shape;

class Hit
{
public:
    Hit();
    Hit(const Ray& i,const Shape *s,const Point& p,const Vector& t,
        const Vector& n=Vector::null);
    virtual ~Hit();

public:
    virtual Ray getReflect()const;
    virtual Ray getRefract(double d1,double d2)const;
    virtual bool isNull()const;

public:
    const Ray& getIncident()const
    {
        return incident;
    }
    const Shape* getShape()const
    {
        return s;
    }
    const Point& getPoint()const
    {
        return p;
    }
    const Vector& getThNormal()const
    {
        return thNorm;
    }
    const Vector& getNormal()const
    {
        return normal;
    }
    int getId()const
    {
        return id;
    }

public:
    void setShape(const Shape* s)
    {
        this->s=s;
    }
    void setNormal(const Vector& n)
    {
        this->normal=n;
    }
    void setThNormal(const Vector& t)
    {
        this->thNorm=t;
        setNormal(t);
    }
    void setId(int i)
    {
        this->id=i;
    }

public:
    static const Hit null;

protected:
    const Shape *s;
    Point p;
    Ray incident;
    Vector thNorm,normal;
    int id;
};

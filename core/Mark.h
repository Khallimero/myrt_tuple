#pragma once

#include "Rotation.h"
#include "Vector.h"
#include "Point.h"
#include "Ray.h"

class Mark
{
public:
    Mark();
    Mark(const Point& o,const Rotation& r=Rotation::null);
    virtual ~Mark();

public:
    Point fromRef(const Point& p)const;
    Point toRef(const Point& p)const;
    Vector fromRef(const Vector& v)const;
    Vector toRef(const Vector& v)const;
    Ray fromRef(const Ray& r)const;
    Ray toRef(const Ray& r)const;

public:
    Mark operator+(const Point& p)const;

public:
    const Point& getOrig()const
    {
        return orig;
    }
    const Rotation& getRot()const
    {
        return rot;
    }

public:
    static Mark Ref;

protected:
    Point orig;
    Rotation rot;
};

#pragma once

#include "Plan.h"

class Disc:public Plan
{
public:
    Disc(double rad,
         double p,const Point& o,
         const Mark& mk);
    virtual ~Disc();

public:
    virtual bool intersect(const Ray& r)const;

protected:
    virtual Hit _getHit(const Ray& r)const;

protected:
    double rad;
};

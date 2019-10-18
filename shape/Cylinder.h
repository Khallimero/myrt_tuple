#pragma once

#include "QuadricShape.h"

class Cylinder:public QuadricShape
{
public:
    Cylinder(double r,const Mark& mk=Mark::Ref);
    Cylinder(const Treble<double>& c,double r,double size,const Mark& mk=Mark::Ref);
    virtual ~Cylinder();
};

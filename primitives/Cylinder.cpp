#include "Cylinder.h"

#include <math.h>

Cylinder::Cylinder(double r,const Mark& mk)
    :QuadricShape(Treble<double>(1,0,1),r,1.0,mk) {}

Cylinder::Cylinder(const Treble<double>& c,double r,double size,const Mark& mk)
    :QuadricShape(c,r,size,mk)
{
    _init();
}

Cylinder::~Cylinder() {}

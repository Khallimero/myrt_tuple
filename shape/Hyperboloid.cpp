#include "Hyperboloid.h"

Hyperboloid::Hyperboloid(const Treble<double>& c,double r,double size,const Mark& mk)
    :QuadricShape(c,r,size,mk)
{
    _init();
    this->coeff*=Treble<double>(1,-1,1);
}

Hyperboloid::~Hyperboloid() {}

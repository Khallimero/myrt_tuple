#include "Cone.h"

Cone::Cone(const Treble<double>& c,double size,const Mark& mk)
    :QuadricShape(c,0.0,size,mk)
{
    _init();
    this->coeff*=Treble<double>(1,-1,1);
}

Cone::~Cone() {}

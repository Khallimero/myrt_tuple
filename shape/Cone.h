#pragma once

#include "QuadricShape.h"

class Cone:public QuadricShape
{
public:
    Cone(const Treble<double>& c,double size,const Mark& mk=Mark::Ref);
    virtual ~Cone();
};

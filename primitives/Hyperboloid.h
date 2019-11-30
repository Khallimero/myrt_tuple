#pragma once

#include "QuadricShape.h"

class Hyperboloid:public QuadricShape
{
public:
    Hyperboloid(const Treble<double>& c,double r,double size,const Mark& mk=Mark::Ref);
    virtual ~Hyperboloid();
};

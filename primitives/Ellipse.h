#pragma once

#include "QuadricShape.h"

class Ellipse:public QuadricShape
{
public:
    Ellipse(const Treble<double>& c,double r,double size,const Mark& mk=Mark::Ref);
    virtual ~Ellipse();

public:
    virtual Point getRelativePoint(const Hit& h,Mapping mp=Undefined)const;
};

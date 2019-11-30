#pragma once

#include "ParametricPrimitive.h"

class Helix:public ParametricPrimitive<1>
{
public:
    Helix(double r,double rise);
    virtual ~Helix();

public:
    virtual Point getPoint(const Tuple<double,1>& t)const;
    virtual SmartPointer<Shape> getBox(const TShapeBuilder<1>* builder,const Mark& mk)const;

protected:
    double r,rise;
};

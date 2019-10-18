#pragma once

#include "ParametricPrimitive.h"

class Riemann:public ParametricPrimitive<1>
{
public:
    Riemann(double rMin,double rMax,
            double s,int p,double c);
    virtual ~Riemann();

public:
    virtual Point getPoint(const Tuple<double,1>& tpl)const;
    virtual double getRad(const Tuple<double,1>& tpl)const;

public:
    virtual SmartPointer<Shape> getBox(const TShapeBuilder<1>* builder,const Mark& mk)const;

protected:
    double rMin,rMax;
    double s,c;
    int p;
};

#pragma once

#include "FractalShape.h"

class Mandelbulb:public FractalShape
{
public:
    Mandelbulb(int pwr,double bov,int iter,
               double size,const Mark& mk=Mark::Ref);
    virtual ~Mandelbulb();

protected:
    virtual double getValue(const Point& p)const;

protected:
    int pwr;
    double bov;
};

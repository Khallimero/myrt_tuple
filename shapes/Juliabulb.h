#pragma once

#include "FractalShape.h"

class Juliabulb:public FractalShape
{
public:
    Juliabulb(const Point& c,
              int pwr,double bov,int iter,
              double size,const Mark& mk=Mark::Ref);
    virtual ~Juliabulb();

protected:
    virtual double getValue(const Point& p)const;

protected:
    Point c;
    int pwr;
    double bov;
};

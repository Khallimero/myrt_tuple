#pragma once

#include "Texture.h"
#include "PerlinNoise.h"

class PerlinPattern:public Texture
{
public:
    PerlinPattern(double a,double b,int n,double f=1.0);
    virtual ~PerlinPattern();

protected:
    PerlinNoise pn;
    double alpha,beta;
    int n;
    double fct;
};

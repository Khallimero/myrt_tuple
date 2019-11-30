#include "PerlinPattern.h"

PerlinPattern::PerlinPattern(double a,double b,int n,double f):Texture()
{
    this->alpha=a,this->beta=b;
    this->n=n;
    this->fct=f;
}

PerlinPattern::~PerlinPattern() {}

#include "CustomPerlinPattern.h"

#include <math.h>

CustomPerlinPattern::CustomPerlinPattern(SmartPointer<const Texture> t1,SmartPointer<const Texture> t2,double a,double b,int n,double f)
    :PerlinPattern(a,b,n,f)
{
    this->t1=t1,this->t2=t2;
}

CustomPerlinPattern::~CustomPerlinPattern() {}

Color CustomPerlinPattern::getColor(const Point& c)const
{
    Point p=c/fct;
    double f=cos(pn.perlinNoiseAbs(p,alpha,beta,n)*10.0);
    f=MAX(0.0,f);
    f=MIN(1.0,f);
    return t1->getColor(c)*f+t2->getColor(c)*(1.0-f);
}

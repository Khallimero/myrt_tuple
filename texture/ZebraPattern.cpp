#include "ZebraPattern.h"

#include <math.h>

ZebraPattern::ZebraPattern(SmartPointer<const Texture> t1,SmartPointer<const Texture> t2,double f)
    :PerlinPattern(5,5,1,f)
{
    this->t1=t1,this->t2=t2;
}

ZebraPattern::~ZebraPattern() {}

Color ZebraPattern::getColor(const Point& c)const
{
    Point p=c*(1.0/fct);
    double f=cos(pn.perlinNoiseAbs(p,alpha,beta,n)*10.0);
    return t1->getColor(c)*f+t2->getColor(c)*(1.0-f);
}

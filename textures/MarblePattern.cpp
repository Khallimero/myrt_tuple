#include "MarblePattern.h"

#include <math.h>

MarblePattern::MarblePattern(SmartPointer<const Texture> t1,SmartPointer<const Texture> t2,double f)
    :PerlinPattern(5,5,5,f),t1(t1),t2(t2)
{}

MarblePattern::~MarblePattern() {}

Color MarblePattern::getColor(const Point& c) const
{
    Point p=c/fct;
    double f=cos(p.getX()+pn.perlinNoiseAbs(p,alpha,beta,n)*10.0);
    return t1->getColor(c)*f+t2->getColor(c)*(1.0-f);
}

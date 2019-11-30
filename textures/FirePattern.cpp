#include "FirePattern.h"

FirePattern::FirePattern(SmartPointer<const Texture> t1,SmartPointer<const Texture> t2,double f)
    :PerlinPattern(0.5,0.9,5,f)
{
    this->t1=t1,this->t2=t2;
}

FirePattern::~FirePattern() {}

Color FirePattern::getColor(const Point& c) const
{
    Point p=c/fct;
    double f=pn.perlinNoiseAbs(p,alpha,beta,n);
    return t1->getColor(c)*f+t2->getColor(c)*(1.0-f);
}

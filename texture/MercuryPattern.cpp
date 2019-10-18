#include "MercuryPattern.h"

MercuryPattern::MercuryPattern(SmartPointer<const Texture> t1,SmartPointer<const Texture> t2,SmartPointer<const Texture> t3,double f)
    :PerlinPattern(2,5,2,f)
{
    this->t1=t1,this->t2=t2,this->t3=t3;
}

MercuryPattern::~MercuryPattern() {}

Color MercuryPattern::getColor(const Point& c)const
{
    static const double s1=0.001,s2=0.4,s3=0.6;
    Point p=c/fct;

    double f=pn.perlinNoiseAbs(p,alpha,beta,n);
    if(f<s2)
        return t1->getColor(c)*((f-s1)/(s2-s1))+t2->getColor(c)*((s2-f)/(s2-s1));
    else if(f<s3)
        return t2->getColor(c)*((f-s2)/(s3-s2))+t3->getColor(c)*((s3-f)/(s3-s2));
    else
        return t3->getColor(c);
}

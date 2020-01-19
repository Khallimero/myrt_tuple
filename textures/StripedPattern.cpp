#include "StripedPattern.h"

StripedPattern::StripedPattern(SmartPointer<const Texture> t1,SmartPointer<const Texture> t2,const Vector& v,long n)
    :Texture(),t1(t1),t2(t2),v(v),n(n)
{}

StripedPattern::~StripedPattern() {}

Color StripedPattern::getColor(const Point& p)const
{
    struct pattern_binary_fct:public binary_fct<double,double>
    {
        virtual void operator()(const double& t1,const double& t2)
        {
            (**this)+=t1*t2;
        }
    } _binary_fct;
    return (long)p.tamper<double>(this->v,_binary_fct)%n==0L?
           t1->getColor(p):t2->getColor(p);
}

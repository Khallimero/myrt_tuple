#include "ChequeredPattern.h"

ChequeredPattern::ChequeredPattern(SmartPointer<const Texture> t1,SmartPointer<const Texture> t2,const Vector& v)
    :Texture(),t1(t1),t2(t2),v(v)
{}

ChequeredPattern::~ChequeredPattern() {}

Color ChequeredPattern::getColor(const Point& p)const
{
    struct pattern_binary_fct:public binary_fct<double,long>
    {
        virtual void operator()(const double& t1,const double& t2)
        {
            double x=t1*t2;
            (**this)+=((long)x)+(x<0?1:0);
        }
    } _binary_fct;
    return p.tamper<long>(this->v,_binary_fct)%2L==0L?
           t1->getColor(p):t2->getColor(p);
}

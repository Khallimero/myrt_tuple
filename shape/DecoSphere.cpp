#include "DecoSphere.h"
#include "Sphere.h"

#include <math.h>

DecoSphere::DecoSphere(double size,const Mark& mk)
    :IsoSurface(size,SmartPointer<const Shape>(new Sphere((M_PI+2.0)+EPSILON)),mk) {}

DecoSphere::~DecoSphere() {}

double DecoSphere::getValue(const Point& p)const
{
    struct decosphere_tuple_idx_fct:public tuple_idx_fct<double,TREBLE_SIZE>
    {
        decosphere_tuple_idx_fct(const Point& p):p(p) {}
        virtual void operator()(const tuple_idx<TREBLE_SIZE>& i)
        {
            double pi0=p(i),pi1=GOLD_NB*p(i+1);
            (**this)+=cos(pi0+pi1)+cos(pi0-pi1);
        }
        const Point& p;
    } _tuple_idx_fct(p);
    return 2.0-_tuple_idx_fct.tamper();
}

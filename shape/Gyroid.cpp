#include "Gyroid.h"

#include <math.h>

Gyroid::Gyroid(double c,
               double size,
               const Shape* box,
               const Mark& mk)
    :IsoSurface(size,box,mk)
{
    this->c=c;
}

Gyroid::~Gyroid() {}

double Gyroid::getValue(const Point& p)const
{
    struct gyroid_tuple_idx_fct:public tuple_idx_fct<double,TREBLE_SIZE>
    {
        gyroid_tuple_idx_fct(const Point& p):p(p) {}
        virtual void operator()(const tuple_idx<TREBLE_SIZE>& i)
        {
            (**this)+=cos(p(i))*sin(p(i+1));
        }
        const Point& p;
    } _tuple_idx_fct(p);
    return this->c+_tuple_idx_fct.tamper();
}

#include "FractalShape.h"

#include <math.h>

FractalShape::FractalShape(int iter,double size,SmartPointer<const Shape> box,const Mark& mk)
    :IsoSurface(size,box,mk)
{
    this->iter=iter;
    this->nrmFct=pow(0.1,iter-1);
}

FractalShape::~FractalShape() {}

Vector FractalShape::getNormal(const Point& p)const
{
    struct vector_unary_idx_op:public unary_idx_op<double,TREBLE_SIZE>
    {
        vector_unary_idx_op(const FractalShape* s,const Point& p):p(p)
        {
            this->s=s;
        }
        virtual double operator()(const tuple_idx<TREBLE_SIZE>& i)const
        {
            return s->getValue(p+(*Vector::EpsilonTab[i])*s->nrmFct)
                   -s->getValue(p-(*Vector::EpsilonTab[i])*s->nrmFct);
        }
        const FractalShape* s;
        const Point& p;
    };

    return Vector(vector_unary_idx_op(this,p));
}

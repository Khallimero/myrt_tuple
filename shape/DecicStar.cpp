#include "DecicStar.h"
#include "Sphere.h"

#include <math.h>

DecicStar::DecicStar(double size,const Mark& mk)
    :IsoSurface(size,SmartPointer<const Shape>(new Sphere(1.5)),mk) {}

DecicStar::~DecicStar() {}

double DecicStar::getValue(const Point& p)const
{
    struct _unary_idx_op1:public unary_idx_op<double,TREBLE_SIZE>
    {
        _unary_idx_op1(const Point& _p):p(_p) {}
        virtual double operator()(const tuple_idx<TREBLE_SIZE>& i)const
        {
            static const double GOLD_NB_P4=pow(GOLD_NB,4);
            return SQ(p(i))-GOLD_NB_P4*SQ(p(i+1));
        }
        const Point& p;
    };

    struct _unary_idx_op2:public unary_idx_op<double,TREBLE_SIZE>
    {
        _unary_idx_op2(const Point& _p):p(_p) {}
        virtual double operator()(const tuple_idx<TREBLE_SIZE>& i)const
        {
            return 2*SQ(p(i))*SQ(p(i+1));
        }
        const Point& p;
    };

    const Point p1=Point(Tuple<double,TREBLE_SIZE>(_unary_idx_op1(p)));
    const Point p2=Point(Tuple<double,TREBLE_SIZE>(_unary_idx_op2(p)));
    const double d=SQ(sumSq(p)-1);

    return 0.5-8.0*prod(p1)*(sum(sq(sq(p)))-sum(p2))
           -(3.0+5.0*GOLD_NB)*d*SQ(sumSq(p)-(1.6-GOLD_NB))-d;
}

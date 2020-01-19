#include "DecoCube.h"
#include "Sphere.h"
#include "SubShape.h"

struct _unary_idx_op1:public unary_idx_op<double,TREBLE_SIZE>
{
    _unary_idx_op1(const Point& p,double c) : _p(p),_c(c) {}
    virtual double operator()(const tuple_idx<TREBLE_SIZE>& i)const
    {
        return _p(i)+_p(i+1)-_c;
    }
    Point _p;
    double _c;
};

struct _unary_idx_op2:public unary_idx_op<double,TREBLE_SIZE>
{
    _unary_idx_op2(const Point& p):_p(p) {}
    virtual double operator()(const tuple_idx<TREBLE_SIZE>& i)const
    {
        return _p(i+2)-1.;
    }
    Point _p;
};

struct _unary_idx_op:public unary_idx_op<double,TREBLE_SIZE>
{
    _unary_idx_op(const Point& p,double c)
    {
        const Point pp=sq(p);
        this->p1=Point(Tuple<double,TREBLE_SIZE>(_unary_idx_op1(pp,c)));
        this->p2=Point(Tuple<double,TREBLE_SIZE>(_unary_idx_op2(pp)));
        this->r=sq(p1)+sq(p2);
    }
    virtual double operator()(const tuple_idx<TREBLE_SIZE>& i)const
    {
        return r(i);
    }
    Point p1,p2,r;
};

DecoCube::DecoCube(double c1,double c2,double size,const Mark& mk)
    :IsoSurface(size,SmartPointer<const Shape>(new SubShape(new Sphere(3.),new Sphere(2.))),mk)
{
    this->c1=SQ(c1),this->c2=c2;
}

DecoCube::~DecoCube() {}

Point DecoCube::_getPoint(const Point& p)const
{
    return Point(Tuple<double,TREBLE_SIZE>(_unary_idx_op(p,c1)));
}

double DecoCube::getValue(const Point& p)const
{
    return prod(_getPoint(p))-c2;
}

Vector DecoCube::getNormal(const Point& p)const
{
    struct tuple_unary_idx_op:public unary_idx_op<double,TREBLE_SIZE>
    {
        tuple_unary_idx_op(const Tuple<double,TREBLE_SIZE>& p):_p(p) {}
        virtual double operator()(const tuple_idx<TREBLE_SIZE>& i)const
        {
            return _p(i+1)*_p(i+2);
        }
        const Tuple<double,TREBLE_SIZE>& _p;
    };

    _unary_idx_op op(p,c1);
    Point _p=Point(tuple_unary_idx_op(Tuple<double,TREBLE_SIZE>(op)));

    struct vector_unary_idx_op:public unary_idx_op<double,TREBLE_SIZE>
    {
        struct point_unary_idx_op:public unary_idx_op<double,TREBLE_SIZE>
        {
            point_unary_idx_op(const _unary_idx_op& op,const tuple_idx<TREBLE_SIZE>& i)
                :_op(op),_i(i) {}
            virtual double operator()(const tuple_idx<TREBLE_SIZE>& i)const
            {
                return (i==_i)?_op.p2(i):_op.p1(i);
            }
            const _unary_idx_op& _op;
            const tuple_idx<TREBLE_SIZE>& _i;
        };

        vector_unary_idx_op(const _unary_idx_op& op,const Point& p)
            :_op(op),_p(p) {}
        virtual double operator()(const tuple_idx<TREBLE_SIZE>& i)const
        {
            return sum(Point(Point(point_unary_idx_op(_op,i+1))*_p));
        }
        const _unary_idx_op& _op;
        const Point& _p;
    };

    return Vector(vector_unary_idx_op(op,_p))*p;
}

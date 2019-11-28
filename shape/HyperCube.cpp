#include "HyperCube.h"

#include <math.h>

HyperCube::HyperCube(double fct,double size,
                     const Mark& mk)
    :PolyShape<HYPERCUBE_POLY_ORDER>(size,mk)
{
    this->fct=fct;
}

HyperCube::~HyperCube() {}

Hit HyperCube::_getHit(const Ray& r)const
{
    Ray rm=mark.fromRef(r);
    double u=_intersectCoeff(rm);
    if(u>0.0)
    {
        static struct normal_unary_op:public unary_op<double,double>
        {
            virtual double operator()(const volatile double &d)const
            {
                return 4.0*CB(d)-10.0*d;
            }
        } _normal_unary_op;
        Point p=Point(rm.getPoint()/size)+(rm.getVector()*(u/size));
        Vector n=mark.toRef(Vector(Tuple<double,TREBLE_SIZE>(p,_normal_unary_op))).norm();
        return Hit(r,this,r.getPoint()+(r.getVector()*u),n);
    }
    return Hit::null;
}

TPoly<HYPERCUBE_POLY_ORDER> HyperCube::_getPoly(const Point& p,const Vector& v)const
{
    struct poly_binary_fct:public binary_fct< double,TPoly<HYPERCUBE_POLY_ORDER> >
    {
        poly_binary_fct(double f)
        {
            (**this)[0]=f;
        }
        virtual void operator()(const double& d1,const double& d2)
        {
            TPoly<1> p1;
            TPoly<1>::initFromCoeffList(&p1,d1,d2);
            TPoly<2> p2=p1.sq();
            (**this)-=p2*5;
            (**this)+=p2.sq();
        }
    };

    poly_binary_fct _binary_fct(this->fct);
    return p.tamper(v,_binary_fct);
}

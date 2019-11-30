#include "HyperTriangle.h"

#include <math.h>

HyperTriangle::HyperTriangle(double a,double b,double k,
                             double size,
                             const Mark& mk)
    :PolyShape<HYPERTRIANGLE_POLY_ORDER>(size,mk)
{
    this->a=a,this->b=b,this->k=k;
}

HyperTriangle::~HyperTriangle() {}

Hit HyperTriangle::_getHit(const Ray& r)const
{
    Ray rm=mark.fromRef(r);
    double u=_intersectCoeff(rm);
    if(u>0.0)
    {
        Point p=Point(rm.getPoint()/size)+(rm.getVector()*(u/size));
        double d=sumSq(p)-a*SQ(k);
        double d1=b*(SQ(p.getZ()+k)-2.0*SQ(p.getY()));
        double d2=b*(SQ(p.getZ()-k)-2.0*SQ(p.getX()));
        Vector n=mark.toRef(Vector(Treble<double>(d+d1,d+d2,d)*4.0*p-Vector::UnitZ*(2.0*((d1+d2)*p.getZ()-k*(d1-d2))))).norm();
        return Hit(r,this,r.getPoint()+(r.getVector()*u),n);
    }
    return Hit::null;
}

TPoly<HYPERTRIANGLE_POLY_ORDER> HyperTriangle::_getPoly(const Point& p,const Vector& v)const
{
    poly_binary_sum<TREBLE_SIZE> _binary_sum(-a*SQ(k));
    TPoly<HYPERTRIANGLE_POLY_ORDER> pr=p.tamper(v,_binary_sum).sq();
    TPoly<2> p1=TPoly<1>(_binary_sum.poly[ITreble::third]-TPoly<0>(k)).sq()-_binary_sum.sqPoly[ITreble::first]*2.0;
    TPoly<2> p2=TPoly<1>(_binary_sum.poly[ITreble::third]+TPoly<0>(k)).sq()-_binary_sum.sqPoly[ITreble::second]*2.0;
    return pr-TPoly<4>(&p1,&p2,&TPoly<4>::setMul)*b;
}

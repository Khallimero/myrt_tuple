#include "Tore.h"

const Vector Tore::Unit=Vector(1,0,1);

Tore::Tore(double r1,double r2,
           const Mark& mk)
    :PolyShape<TORE_POLY_ORDER>(1.0,mk),sBox(r1+r2,mk)
{
    this->r1=r1,this->r2=r2;
}

Tore::~Tore() {}

Point Tore::getRelativePoint(const Hit& h,Mapping mp)const
{
    if(mp==Planar)
    {
        Point a=mark.fromRef(h.getPoint());
        Point o=Vector(a*Tore::Unit).norm()*r1;
        double x=atan2(sqrt(SQ(a.getX())+SQ(a.getZ()))-sqrt(SQ(o.getX())+SQ(o.getZ())),
                       a.getY()-o.getY())/(M_PI*2.0)+0.5;
        double y=atan2(a.getX(),a.getZ())/(M_PI*2.0)+0.5;
        double z=a.dist(o)/r2;
        return Point(x,y,z);
    }

    return Shape::getRelativePoint(h,mp);
}

bool Tore::isInside(const Point& p,double e)const
{
    double d=mark.getOrig().dist(p);
    if(d>r1+r2+e||d<r1-r2-e)return false;
    Point a=mark.fromRef(p);
    Point o=Vector(a*Tore::Unit).norm()*r1;
    return o.dist(a)<=r2+e;
}

bool Tore::intersect(const Ray& r)const
{
    if(!sBox.intersect(r))return false;
    return PolyShape<TORE_POLY_ORDER>::intersect(r);
}

Hit Tore::_getHit(const Ray& r)const
{
    if(!sBox.intersect(r))return Hit::null;
    Ray rm=mark.fromRef(r);
    double u=_intersectCoeff(rm);
    if(u>0.0)
    {
        static const struct unit_unary_op:public unary_op<double,double>
        {
            virtual double operator()(const volatile double& d)const
            {
                return d>0?1:-1;
            }
        } _unit_unary_op;
        static const Tuple<double,TREBLE_SIZE> cf(Tore::Unit,_unit_unary_op);

        const Point p=rm.getPoint()+(rm.getVector()*u);
        double d=4*(sumSq(p)-SQ(this->r2));
        Vector v=cf*(4*SQ(this->r1))*p;
        Vector n=mark.toRef(Vector(p*d-v)).norm();

        return Hit(r,this,Point(r.getPoint()+(r.getVector()*u)),n);
    }
    return Hit::null;
}

TPoly<TORE_POLY_ORDER> Tore::_getPoly(const Point& p,const Vector& v)const
{
    struct poly_idx_fct:public tuple_idx_fct<TPoly<2>,TREBLE_SIZE>
    {
        poly_idx_fct(const TPoly<2>* _p)
        {
            this->p=_p;
        }
        virtual void operator()(const tuple_idx<TREBLE_SIZE>& i)
        {
            (**this)+=p[i]*Tore::Unit(i);
        }
        const TPoly<2>* p;
    };

    poly_binary_sum<TREBLE_SIZE> _binary_sum(SQ(this->r1)-SQ(this->r2));
    TPoly<TORE_POLY_ORDER> pr=p.tamper(v,_binary_sum).sq();
    poly_idx_fct _idx_fct(_binary_sum.sqPoly);
    TPoly<2> p2=_idx_fct.tamper();
    return pr-p2*(4.0*SQ(this->r1));
}

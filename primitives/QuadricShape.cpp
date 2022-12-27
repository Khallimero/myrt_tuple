#include "QuadricShape.h"
#include "Poly.h"

QuadricShape::QuadricShape(const Treble<double>& c,double r,double size,const Mark& mk)
    :PolyShape<QUADRICSHAPE_POLY_ORDER>(size,mk),
     coeff(c),radius(r)
{}

QuadricShape::~QuadricShape() {}

bool QuadricShape::isInside(const Point& h,double e)const
{
    Point p=mark.fromRef(h)/size;
    return Geometric(p*p).prodScal(this->coeff)<SQ(radius)+e;
}

Point QuadricShape::getRelativePoint(const Hit& h,Mapping mp)const
{
    if(mp==Planar)
    {
        Point p=mark.fromRef(h.getPoint());
        double x=atan2(p.getX(),p.getZ())/(M_PI*2.0)+0.5;
        double y=p.getY();
        double z=Vector(p*Vector(1,0,1)).length()/(radius*size);
        return Point(x,y,z);
    }

    return Shape::getRelativePoint(h,mp);
}

Hit QuadricShape::_getHit(const Ray& r)const
{
    Ray rm=mark.fromRef(r);
    double u=_intersectCoeff(rm);
    if(u>0.0)
    {
        Point p=Point(rm.getPoint()/size)+(rm.getVector()*(u/size));
        Vector n=mark.toRef(Vector(p*coeff*2.0)).norm();
        return Hit(r,this,r.getPoint()+(r.getVector()*u),n);
    }
    return Hit::null;
}

TPoly<QUADRICSHAPE_POLY_ORDER> QuadricShape::_getPoly(const Point& p,const Vector& v)const
{
    struct poly_idx_fct:public tuple_idx_fct<TPoly<QUADRICSHAPE_POLY_ORDER>,TREBLE_SIZE>
    {
        poly_idx_fct(const Point& _p,const Vector& _v,const Treble<double>& c,double f)
            :p(_p),v(_v),coeff(c)
        {
            (**this)[0]=f;
        }
        virtual void operator()(const tuple_idx<TREBLE_SIZE>& i)
        {
            TPoly<1> poly;
            TPoly<1>::initFromCoeffList(&poly,p(i),v(i));
            (**this)+=poly.sq()*coeff(i);
        }

        const Point& p;
        const Vector& v;
        const Treble<double>& coeff;
    };

    poly_idx_fct _poly_idx_fct(p,v,coeff,-SQ(radius));
    return _poly_idx_fct.tamper();
}

#include "IsoSurface.h"

#include <math.h>

IsoSurface::IsoSurface(double size,SmartPointer<const Shape> box,const Mark& mk)
    :Shape(mk),size(size),box(box)
{}

IsoSurface::~IsoSurface() {}

bool IsoSurface::isInside(const Point& i,double e)const
{
    Point p=mark.fromRef(i)/size;
    if(!box->isInside(p))return false;
    return getValue(p)<(e+ISO_THRESHOLD);
}

Hit IsoSurface::_getHit(const Ray& r)const
{
    Point p=mark.fromRef(r.getPoint())/size;

    do
    {
        Vector u=mark.fromRef(r.getVector());

        if(!box->isInside(p))
        {
            Hit h=box->getHit(Ray(p,u));
            if(h.isNull())return Hit::null;
            p=h.getPoint();
        }

        double l=ISO_STEP;
        u=u.norm()*l;

        double v0=getValue(p);
        do
        {
            p+=u;
            double v=getValue(p);
            if(fabs(v)<=ISO_THRESHOLD||l<ISO_THRESHOLD)
            {
                Vector n=Vector(mark.toRef(getNormal(p))).norm();
                return Hit(r,this,mark.toRef((Point)(p*size)),n);
            }
            if(v0*v<0)u/=-2.0,l/=2.0,v0=v;
        }
        while(box->isInside(p));
    }
    while(true);
}

Vector IsoSurface::getNormal(const Point& p)const
{
    struct vector_unary_idx_op:public unary_idx_op<double,TREBLE_SIZE>
    {
        vector_unary_idx_op(const IsoSurface* s,const Point& p):p(p)
        {
            this->s=s;
        }
        virtual double operator()(const tuple_idx<TREBLE_SIZE>& i)const
        {
            return s->getValue(p+Vector::EpsilonTab[i])-s->getValue(p-Vector::EpsilonTab[i]);
        }
        const IsoSurface* s;
        const Point& p;
    };

    return Vector(vector_unary_idx_op(this,p));
}

#include "Disc.h"

Disc::Disc(double rad,
           double p,const Point& o,
           const Mark& mk)
    :Plan(p,o,mk)
{
    this->rad=rad;
}

Disc::~Disc() {}

bool Disc::intersect(const Ray& r)const
{
    return Shape::intersect(r);
}

Hit Disc::_getHit(const Ray& r)const
{
    double l=_intersectCoeff(r);
    if(l>0)
    {
        Point h=r.getPoint()+(r.getVector()*l);
        if(h.dist(orig)<rad)
            return Hit(r,this,h,getNormal());
    }
    return Hit::null;
}

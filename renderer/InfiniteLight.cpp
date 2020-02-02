#include "InfiniteLight.h"
#include "Hit.h"

InfiniteLight::InfiniteLight(const Vector& v,const Color& c)
    :Light(c),v(v)
{}

InfiniteLight::~InfiniteLight() {}

Vector InfiniteLight::getVectorTo(const Point& p)const
{
    return -v;
}

Point InfiniteLight::getOrig(const Shape* s,const Point& p)const
{
    Point q=p;
    do
    {
        q+=getVectorTo(p).norm()*EPSILON;
        Ray r=Ray(q,getVectorTo(q));
        Hit h=s->getHit(r);
        if(h.isNull())return q;
        q=h.getPoint();
    }
    while(true);
}

Ray InfiniteLight::getRay(const Shape* s,const Point& p,const Point& o,const NestedIterator<double,2>* it)const
{
    return Ray(getOrig(s,p),v);
}

double InfiniteLight::dist(const Point& p)const
{
    return -1;
}

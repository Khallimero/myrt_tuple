#include "SphericLight.h"

#include <math.h>

SphericLight::SphericLight(double r,const Point& p,const Color& c,double gl)
    :PointLight(p,c,gl)
{
    this->r=r;
    this->box=new Sphere(r,Mark(p));
}

SphericLight::~SphericLight()
{
    delete box;
}

NestedIterator<double,2>* SphericLight::getIterator(int ph)const
{
    return box->getIterator(ph);
}

Ray SphericLight::getRay(const Shape* s,const Point& p,const Point& o,const NestedIterator<double,2>* it)const
{
    Point c=box->getPoint(o,it);
    return Ray(c,c.getVectorTo(p));
}

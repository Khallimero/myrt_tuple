#include "Hit.h"

#include <stdlib.h>

Hit::Hit()
{
    *this=null;
}

Hit::Hit(const Ray& i,const Shape *s,const Point& p,const Vector& t,const Vector& n)
{
    this->incident=i,this->s=s,this->p=p,this->thNorm=t;
    this->normal=(n==Vector::null?t:n);
    this->id=-1;
}

Hit::~Hit() {}

Ray Hit::getReflect()const
{
    Vector r=incident.getVector().reflect(normal).norm();
    return Ray(p+(r*EPSILON),r);
}

Ray Hit::getRefract(double d1,double d2)const
{
    Vector r=incident.getVector().refract(normal,d1,d2);
    return Ray(p+(r.norm()*EPSILON),r);
}

bool Hit::isNull()const
{
    return s==NULL;
}

const Hit Hit::null=Hit(Ray::null,NULL,Point::null,Vector::null);

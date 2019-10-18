#include "Ray.h"

Ray::Ray()
{
    *this=null;
}

Ray::Ray(const Point& p,const Vector& v)
{
    this->p=p,this->v=v;
}

Ray::Ray(const Point& p1,const Point& p2)
{
    this->p=p1;
    this->v=p1.getVectorTo(p2);
}

Ray::~Ray() {}

bool Ray::operator==(const Ray& that)const
{
    return (this->p==that.p)&&(this->v==that.v);
}

double Ray::dist(const Point& t)const
{
    return this->p.getVectorTo(t).prodVect(this->v).length()/this->v.length();
}

Point Ray::proj(const Point& t)const
{
    return this->p+this->p.getVectorTo(t).projOn(this->v);
}

bool Ray::isNull()const
{
    return v.isNull();
}

const Ray Ray::null=Ray(Point::null,Vector::null);

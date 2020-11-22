#include "Triangle.h"

Triangle::Triangle(const Vector& v1,const Vector& v2,const Mark& mk)
    :PlaneShape(mk),
     v1(v1),v2(v2)
{
    orig=mk.getOrig();
}

Triangle::Triangle(const Point& p1,const Point& p2,const Point& p3)
    :PlaneShape(Mark(p1)),
     v1(p1.getVectorTo(p2)),v2(p1.getVectorTo(p3))
{
    orig=p1;
}

Triangle::~Triangle() {}

Hit Triangle::_getHit(const Ray& r)const
{
    return Triangle::getTriangleHit(r,this,orig,v1,v2);
}

Hit Triangle::getTriangleHit(const Ray& r,const Shape* s,const Point* p)
{
    return Triangle::getTriangleHit(r,s,p[0],p[0].getVectorTo(p[1]),p[0].getVectorTo(p[2]));
}

Hit Triangle::getTriangleHit(const Ray& r,const Shape* s,const Point& o,const Vector& v1,const Vector& v2)
{
    double d=v2.prodVect(v1).prodScal(r.getVector());
    Vector w=o.getVectorTo(r.getPoint());
    double a=v2.prodVect(w).prodScal(r.getVector())/d;
    if(a<0.0||a>1.0)return Hit::null;
    double b=w.prodVect(v1).prodScal(r.getVector())/d;
    if(b<0.0||b>1.0)return Hit::null;
    if((a+b)>1.0)return Hit::null;

    double t=v1.prodVect(v2).prodScal(w)/d;
    if(t>0.0)return Hit(r,s,Point(r.getPoint()+(r.getVector()*t)),Triangle::getTriangleNormal(v1,v2));

    return Hit::null;
}

Vector Triangle::getTriangleNormal(const Point* p)
{
    return Triangle::getTriangleNormal(p[0].getVectorTo(p[1]),
                                       p[0].getVectorTo(p[2]));
}

Vector Triangle::getTriangleNormal(const Vector& v1,const Vector& v2)
{
    return v1.prodVect(v2);
}

Vector Triangle::getNormal()const
{
    return Triangle::getTriangleNormal(v1,v2);
}

double Triangle::getArea()const
{
    return v1.prodVect(v2).length()/2.0;
}

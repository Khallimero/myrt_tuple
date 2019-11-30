#include "Sphere.h"

#include <math.h>

Sphere::Sphere(double r,const Mark& mk)
    :Ellipse(Treble<double>(1,1,1),r,1.0,mk)
{
}

Sphere::~Sphere() {}

RoundedIterator* Sphere::getIterator(int ph)const
{
    double s=sqrt(ph);
    return new RoundedIterator(Iterator<double>((int)ceil(s),this->radius*2.0/s));
}

Point Sphere::getPoint(const Point& p,const NestedIterator<double,2>* it)const
{
    return getPoint(p,it->getElement(0),it->getElement(1));
}

Point Sphere::getPoint(const Point& p,const IteratorElement<double>& i,const IteratorElement<double>& j)const
{
    Vector v=mark.getOrig().getVectorTo(p).norm();
    Vector v1=v.getOrtho().norm()*i;
    Vector v2=v.prodVect(v1).norm()*j;
    return mark.getOrig()+(v1+v2);
}

Point Sphere::getRelativePoint(const Hit& h,Mapping mp)const
{
    if(mp==Planar)
    {
        Point a=h.getPoint().centerRotate(mark.getOrig(),mark.getRot());
        double x=atan2(a.getZ()-mark.getOrig().getZ(),a.getX()-mark.getOrig().getX())/(M_PI*2.0)+0.5;
        double y=asin((a.getY()-mark.getOrig().getY())/(radius*size))/(M_PI)+0.5;
        double z=h.getPoint().dist(mark.getOrig())/(radius*size);
        return Point(x,y,z);
    }

    return Shape::getRelativePoint(h,mp);
}

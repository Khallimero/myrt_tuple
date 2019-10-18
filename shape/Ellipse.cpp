#include "Ellipse.h"

Ellipse::Ellipse(const Treble<double>& c,double r,double size,const Mark& mk)
    :QuadricShape(c,r,size,mk)
{
    _init();
}

Ellipse::~Ellipse() {}

Point Ellipse::getRelativePoint(const Hit& h,Mapping mp)const
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

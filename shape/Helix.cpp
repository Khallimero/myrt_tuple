#include "Helix.h"
#include "Cylinder.h"

Helix::Helix(double r,double rise)
{
    this->r=r,this->rise=rise;
}

Helix::~Helix() {}

Point Helix::getPoint(const Tuple<double,1>& t)const
{
    const double a=get(t);
    double x=r*cos(a);
    double y=rise*(a/(M_PI*2.0));
    double z=r*sin(a);

    return Point(x,y,z);
}

SmartPointer<Shape> Helix::getBox(const TShapeBuilder<1>* builder,const Mark& mk)const
{
    return SmartPointer<Shape>(new Cylinder(builder->getSize()*r+builder->getRad()+EPSILON,mk));
}

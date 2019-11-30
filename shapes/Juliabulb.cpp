#include "Juliabulb.h"
#include "Sphere.h"

#include <math.h>

Juliabulb::Juliabulb(const Point& c,
                     int pwr,double bov,int iter,
                     double size,const Mark& mk)
    :FractalShape(iter,size,SmartPointer<const Shape>(new Sphere(1.2)),mk)
{
    this->c=c;
    this->pwr=pwr;
    this->bov=bov;
}

Juliabulb::~Juliabulb() {}

double Juliabulb::getValue(const Point& p)const
{
    Point z=p;
    int i=1;
    while(true)
    {
        double r=sqrt(z);
        if(r>bov||++i>iter)return r-1.0;
        double zr=pow(r,pwr);
        double theta=acos(z.getZ()/r)*(double)pwr;
        double phi=atan2(z.getY(),z.getX())*(double)pwr;
        z=c+Point(sin(theta)*cos(phi),sin(theta)*sin(phi),cos(theta))*zr;
    }
}

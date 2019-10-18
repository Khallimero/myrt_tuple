#include "Mandelbulb.h"
#include "Sphere.h"

#include <math.h>

Mandelbulb::Mandelbulb(int pwr,double bov,int iter,double size,const Mark& mk)
    :FractalShape(iter,size,SmartPointer<const Shape>(new Sphere(1.2)),mk)
{
    this->pwr=pwr;
    this->bov=bov;
}

Mandelbulb::~Mandelbulb() {}

double Mandelbulb::getValue(const Point& p)const
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
        z=p+Point(sin(theta)*cos(phi),sin(theta)*sin(phi),cos(theta))*zr;
    }
}

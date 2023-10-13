#include "Riemann.h"
#include "Sphere.h"

#include <math.h>

Riemann::Riemann(double rMin,double rMax,
                 double s,int p,double c)
{
    this->rMin=rMin,this->rMax=rMax;
    this->s=s,this->p=(2*p+1),this->c=c;
}

Riemann::~Riemann() {}

Point Riemann::getPoint(const Tuple<double,1>& tpl)const
{
    const double a=get(tpl);
    double a1=a/2.0;
    double t=a/M_PI-1.0;
    double a2=(((pow(t,p)-t)*(M_PI*c))+a)*s;

    double x=sin(a1)*cos(a2);
    double y=sin(a1)*sin(a2);
    double z=cos(a1);

    return Point(x,y,z);
}

double Riemann::getRad(const Tuple<double,1>& tpl)const
{
    const double a=get(tpl);
    return rMin+(rMax-rMin)*sin(a/2.0);
}

SmartPointer<Shape> Riemann::getBox(const TShapeBuilder<1>* builder,const Mark& mk)const
{
    return SmartPointer<Shape>(new Sphere(builder->getSize()+rMax+EPSILON,mk));
}

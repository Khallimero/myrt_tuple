#include "Kluchikov.h"
#include "Tore.h"

#include <math.h>

Kluchikov::Kluchikov(double size,const Mark& mk)
    :IsoSurface(size,SmartPointer<const Shape>(new Tore(1.5,0.5)),mk) {}

Kluchikov::~Kluchikov() {}

static const double pi=16.0*M_PI/3.0;
static const double a=cos(M_PI*4.0/6.0)/4.0;
static const double b=sin(M_PI*4.0/6.0)/4.0;
static const double c=1.0/4.0;
static const double pp=1.0/64.0;

double Kluchikov::getValue(const Point& p)const
{
    const double r=(atan2(p.getZ(),p.getX())/(2.0*M_PI))+0.5;
    const double rp=r*pi;
    const double x=sqrt(SQ(p.getX())+SQ(p.getZ()))-1.5;
    const double y=p.getY();
    const double cs=cos(rp);
    const double sn=sin(rp);
    const double px=x*sn+y*cs;
    const double py=x*cs-y*sn;

    return sin(r*10.0*M_PI)*0.01-0.945
           +(pow(SQ(px+c)+SQ(py),pp)
             +pow(SQ(px+a)+SQ(py+b),pp)
             +pow(SQ(px+a)+SQ(py-b),pp))*0.33;
}

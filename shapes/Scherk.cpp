#include "Scherk.h"
#include "Cylinder.h"

#include <math.h>

Scherk::Scherk(double r,double size,const Mark& mk)
    :IsoSurface(size,SmartPointer<const Shape>(new Cylinder(r)),mk) {}

Scherk::~Scherk() {}

double Scherk::getValue(const Point& p)const
{
    return sin(p.getY())-sinh(p.getX())*sinh(p.getZ());
}

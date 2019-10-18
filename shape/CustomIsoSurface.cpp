#include "CustomIsoSurface.h"

CustomIsoSurface::CustomIsoSurface(double (*fct)(const Point& p),
                                   double size,
                                   const Shape* box,
                                   const Mark& mk)
    :IsoSurface(size,box,mk)
{
    this->fct=fct;
}

CustomIsoSurface::~CustomIsoSurface() {}

double CustomIsoSurface::getValue(const Point& p)const
{
    return fct(p);
}

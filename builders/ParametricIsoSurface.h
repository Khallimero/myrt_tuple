#pragma once

#include "IsoSurface.h"
#include "StraightShapeBuilder.h"
#include "ParametricPrimitive.h"
#include "Collection.h"

class ParametricIsoSurface:protected StraightShapeBuilder,public IsoSurface
{
public:
    ParametricIsoSurface(const ParametricPrimitive<1> *prim,
                         double size,double spin,int steps,double rad,
                         const Mark& mk=Mark::Ref);
    virtual ~ParametricIsoSurface();

protected:
    virtual double getValue(const Point& p)const;

protected:
    const ParametricPrimitive<1> *prim;
    ObjCollection<Point> points;
};

#pragma once

#include "IsoSurface.h"
#include "StraightShapeBuilder.h"
#include "ParametricPrimitive.h"
#include "SmartPointer.h"
#include "Collection.h"

class ParametricIsoSurface:protected StraightShapeBuilder,public IsoSurface
{
public:
    ParametricIsoSurface(SmartPointer<const ParametricPrimitive<1>> prim,
                         double size,double spin,int steps,double rad,
                         const Mark& mk=Mark::Ref);
    virtual ~ParametricIsoSurface();

protected:
    virtual double getValue(const Point& p)const;

protected:
    SmartPointer<const ParametricPrimitive<1>> prim;
    ObjCollection<Point> points;
};

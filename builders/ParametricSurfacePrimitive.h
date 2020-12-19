#pragma once

#include "ParametricPrimitive.h"
#include "SmartPointer.h"

class ParametricSurfacePrimitive:public ParametricPrimitive<2>
{
public:
    ParametricSurfacePrimitive(SmartPointer<const ParametricPrimitive<1>> prim);
    virtual ~ParametricSurfacePrimitive();

public:
    virtual Point getPoint(const Tuple<double,2>& t)const;

protected:
    SmartPointer<const ParametricPrimitive<1>> prim;
};

#pragma once

#include "ParametricPrimitive.h"

class ParametricSurfacePrimitive:public ParametricPrimitive<2>
{
public:
    ParametricSurfacePrimitive(const ParametricPrimitive<1> *prim);
    virtual ~ParametricSurfacePrimitive();

public:
    virtual Point getPoint(const Tuple<double,2>& t)const;

protected:
    const ParametricPrimitive<1> *prim;
};

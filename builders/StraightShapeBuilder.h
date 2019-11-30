#pragma once

#include "ShapeBuilder.h"

class StraightShapeBuilder:public ShapeBuilder<1>
{
public:
    StraightShapeBuilder(double size,double spin,int steps,double rad);
    virtual ~StraightShapeBuilder();

public:
    virtual Point getPoint(const Tuple<int,1>& i,const ParametricPrimitive<1>* prim)const;
};

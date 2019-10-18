#pragma once

#include "ShapeBuilder.h"

class CoilingShapeBuilder:public ShapeBuilder<1>
{
public:
    CoilingShapeBuilder(double fct,double coil,double size,double spin,int steps,double rad);
    virtual ~CoilingShapeBuilder();

public:
    virtual Point getPoint(const Tuple<int,1>& i,const ParametricPrimitive<1>* prim)const;
    virtual double getBoxRad()const;

protected:
    double fct,coil;
};

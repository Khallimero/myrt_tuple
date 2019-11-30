#pragma once

#include "ParametricPrimitive.h"

class GrannyKnot:public ParametricPrimitive<1>
{
public:
    GrannyKnot();
    virtual ~GrannyKnot();

public:
    virtual Point getPoint(const Tuple<double,1>& t)const;
};

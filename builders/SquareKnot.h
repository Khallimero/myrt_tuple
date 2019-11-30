#pragma once

#include "ParametricPrimitive.h"

class SquareKnot:public ParametricPrimitive<1>
{
public:
    SquareKnot();
    virtual ~SquareKnot();

public:
    virtual Point getPoint(const Tuple<double,1>& t)const;
};

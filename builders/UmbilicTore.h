#pragma once

#include "ParametricPrimitive.h"

class UmbilicTore:public ParametricPrimitive<2>
{
public:
    UmbilicTore();
    virtual ~UmbilicTore();

public:
    virtual Point getPoint(const Tuple<double,2>& t)const;
};

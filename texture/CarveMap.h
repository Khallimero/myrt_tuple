#pragma once

#include "Point.h"

class CarveMap
{
public:
    CarveMap();
    virtual ~CarveMap();

public:
    virtual bool isCarved(const Point&)const=0;
};

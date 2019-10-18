#pragma once

#include "BumpMap.h"
#include "PerlinNoise.h"

class PerlinBumpMap:public BumpMap
{
public:
    PerlinBumpMap(double f=1.0);
    virtual ~PerlinBumpMap();

public:
    virtual Vector getVector(const Point& p)const;

protected:
    PerlinNoise pn;
};

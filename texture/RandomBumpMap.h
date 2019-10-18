#pragma once

#include "BumpMap.h"
#include "Map.h"
#include "Vector.h"

class RandomBumpMap:public BumpMap
{
public:
    RandomBumpMap(int w,int h,double f=1.0);
    virtual ~RandomBumpMap();

public:
    virtual Vector getVector(const Point& p)const;

protected:
    Map<Vector> m;
};

#pragma once

#include "Color.h"

class Fog
{
public:
    Fog();
    Fog(const Color& c,double d,double p=1.0);
    virtual ~Fog();

public:
    const Color& getColor()const
    {
        return col;
    }
    double getDist()const
    {
        return dst;
    }
    double getPow()const
    {
        return pw;
    }

protected:
    Color col;
    double dst,pw;
};

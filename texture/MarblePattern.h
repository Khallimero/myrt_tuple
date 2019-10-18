#pragma once

#include "PerlinPattern.h"
#include "Texture.h"
#include "Color.h"
#include "Point.h"
#include "SmartPointer.h"

class MarblePattern:public PerlinPattern
{
public:
    MarblePattern(SmartPointer<const Texture> t1,SmartPointer<const Texture> t2,double f=1.0);
    virtual ~MarblePattern();

public:
    virtual Color getColor(const Point& c)const;

protected:
    SmartPointer<const Texture> t1,t2;
};

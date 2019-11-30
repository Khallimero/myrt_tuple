#pragma once

#include "PerlinPattern.h"
#include "Texture.h"
#include "Color.h"
#include "Point.h"
#include "SmartPointer.h"

class CustomPerlinPattern:public PerlinPattern
{
public:
    CustomPerlinPattern(SmartPointer<const Texture> t1,SmartPointer<const Texture> t2,double a,double b,int n,double f=1.0);
    virtual ~CustomPerlinPattern();

public:
    virtual Color getColor(const Point& c)const;

protected:
    SmartPointer<const Texture> t1,t2;
};

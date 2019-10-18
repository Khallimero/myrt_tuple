#pragma once

#include "PerlinPattern.h"
#include "Texture.h"
#include "Color.h"
#include "Point.h"
#include "SmartPointer.h"

class CloudPattern:public PerlinPattern
{
public:
    CloudPattern(SmartPointer<const Texture> t1,SmartPointer<const Texture> t2,SmartPointer<const Texture> t3,double f=1.0);
    virtual ~CloudPattern();

public:
    virtual Color getColor(const Point& c)const;

protected:
    SmartPointer<const Texture> t1,t2,t3;
};

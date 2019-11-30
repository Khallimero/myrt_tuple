#pragma once

#include "BumpMap.h"
#include "Texture.h"
#include "SmartPointer.h"

class TextureBumpMap:public BumpMap
{
public:
    TextureBumpMap(SmartPointer<const Texture> t,double f);
    virtual ~TextureBumpMap();

public:
    virtual Vector getVector(const Point& p)const;

protected:
    SmartPointer<const Texture> t;
};

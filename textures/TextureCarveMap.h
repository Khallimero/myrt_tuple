#pragma once

#include "CarveMap.h"
#include "Texture.h"

class TextureCarveMap:public CarveMap
{
public:
    TextureCarveMap(const Texture* t,double a=.5);
    virtual ~TextureCarveMap();

public:
    virtual bool isCarved(const Point& p)const;

protected:
    const Texture* tex;
    double a;
};

#pragma once

#include "Point.h"
#include "Color.h"

class Texture
{
public:
    Texture();
    virtual ~Texture();

public:
    virtual Color getColor(const Point&)const=0;
};

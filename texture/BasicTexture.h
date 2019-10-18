#pragma once

#include "Texture.h"
#include "Color.h"

class BasicTexture:public Texture
{
public:
    BasicTexture(const Color& c);
    virtual ~BasicTexture();

public:
    virtual Color getColor(const Point&)const
    {
        return c;
    }

protected:
    Color c;
};

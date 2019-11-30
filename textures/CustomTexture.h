#pragma once

#include "Texture.h"

class CustomTexture:public Texture
{
public:
    CustomTexture(Color (*fct)(const Point&));
    virtual ~CustomTexture();

protected:
    virtual Color getColor(const Point&)const;

protected:
    Color (*fct)(const Point&);
};

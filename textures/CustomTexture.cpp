#include "CustomTexture.h"

CustomTexture::CustomTexture(Color (*fct)(const Point&))
    :Texture()
{
    this->fct=fct;
}

CustomTexture::~CustomTexture() {}

Color CustomTexture::getColor(const Point& p)const
{
    return fct(p);
}

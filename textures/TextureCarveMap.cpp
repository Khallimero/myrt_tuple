#include "TextureCarveMap.h"
#include "Color.h"

TextureCarveMap::TextureCarveMap(const Texture* t,double a)
{
    this->tex=t,this->a=a;
}

TextureCarveMap::~TextureCarveMap() {}

bool TextureCarveMap::isCarved(const Point& p)const
{
    return tex->getColor(p).grey()>a;
}

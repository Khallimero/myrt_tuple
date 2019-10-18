#pragma once

#include "Texture.h"
#include "Picture.h"
#include "Color.h"
#include "Point.h"
#include "SmartPointer.h"

class PictureTexture:public Texture
{
public:
    PictureTexture(SmartPointer<const Picture> p);
    PictureTexture(const char* fileName);
    virtual ~PictureTexture();

public:
    virtual Color getColor(const Point& c)const;

protected:
    SmartPointer<const Picture> p;
};

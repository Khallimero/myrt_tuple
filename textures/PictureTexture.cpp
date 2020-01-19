#include "PictureTexture.h"

PictureTexture::PictureTexture(SmartPointer<const Picture> p)
    :Texture(),p(p)
{}

PictureTexture::PictureTexture(const char* fileName)
    :Texture(),p(new Picture(fileName))
{}

PictureTexture::~PictureTexture() {}

Color PictureTexture::getColor(const Point& c)const
{
    return p->getElem(c);
}

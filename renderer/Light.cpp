#include "Light.h"

Light::Light(const Color& c,double gl)
{
    this->c=c,this->glare=gl;
}

Light::~Light() {}

NestedIterator<double,2>* Light::getIterator(int ph)const
{
    return new NestedIterator<double,2>(Iterator<double>(1,0),2);
}

#include "Light.h"

Light::Light(const Color& c)
    :c(c)
{}

Light::~Light() {}

NestedIterator<double,2>* Light::getIterator(int)const
{
    return new NestedIterator<double,2>(Iterator<double>(1,0));
}

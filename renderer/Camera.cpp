#include "Camera.h"

Camera::Camera() {}

Camera::Camera(const Point& o,const Vector& v,const Enumeration<Vector>& vaX,const Enumeration<Vector>& vaY)
    :orig(o),dir(v),vaX(vaX),vaY(vaY)
{
    setFocus(this->dir.length()*10.0);
}

Camera::~Camera() {}

Vector Camera::getVector(const IteratorElement<double>& i,const IteratorElement<double>& j)const
{
    return getVaX().getElem(i)+getVaY().getElem(j);
}

#pragma once

#include "Point.h"
#include "Vector.h"
#include "Enumeration.h"
#include "IteratorElement.h"

class Camera
{
public:
    Camera();
    Camera(const Point& o,const Vector& v,const Enumeration<Vector>& vaX,const Enumeration<Vector>& vaY);
    virtual ~Camera();

public:
    void setFocus(double d,double l=-1.)
    {
        this->fcsDst=d,this->lenSize=l;
    }
    double getFocusDist()const
    {
        return fcsDst;
    }
    double getLenSize()const
    {
        return lenSize;
    }
    const Point& getOrig()const
    {
        return orig;
    }
    const Vector& getDir()const
    {
        return dir;
    }
    const Enumeration<Vector>& getVaX()const
    {
        return vaX;
    }
    const Enumeration<Vector>& getVaY()const
    {
        return vaY;
    }

    Vector getVector(const IteratorElement<double>& i,const IteratorElement<double>& j)const;

protected:
    Point orig;
    Vector dir;
    Enumeration<Vector> vaX,vaY;
    double fcsDst;
    double lenSize;
};

#pragma once

#include "Vector.h"
#include "Point.h"
#include "Color.h"

class PhotonHit
{
public:
    PhotonHit();
    PhotonHit(const Point& p,const Vector& v,const Color& c);
    virtual ~PhotonHit();

public:
    bool operator==(const PhotonHit& that)const;
    void addColor(const Color& col);

public:
    const Point& getPoint()const
    {
        return p;
    }
    const Vector& getVector()const
    {
        return v;
    }
    const Color& getColor()const
    {
        return c;
    }

protected:
    Point p;
    Vector v;
    Color c;
};

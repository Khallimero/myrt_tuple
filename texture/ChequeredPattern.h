#pragma once

#include "Texture.h"
#include "Vector.h"
#include "Point.h"
#include "Color.h"
#include "SmartPointer.h"

class ChequeredPattern:public Texture
{
public:
    ChequeredPattern(SmartPointer<const Texture> t1,SmartPointer<const Texture> t2,const Vector& v);
    virtual ~ChequeredPattern();

public:
    virtual Color getColor(const Point& p)const;

protected:
    SmartPointer<const Texture> t1,t2;
    Vector v;
};

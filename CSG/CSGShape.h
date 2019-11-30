#pragma once

#include "Shape.h"
#include "SmartPointer.h"

#ifndef MAX_REC
#define MAX_REC 250
#endif

class CSGShape:public Shape
{
public:
    CSGShape(SmartPointer<const Shape> s1,SmartPointer<const Shape> s2,bool merge=true);
    virtual ~CSGShape();

public:
    void setBox(SmartPointer<const Shape> b)
    {
        box=b;
    }

    bool getMerge()const
    {
        return merge;
    }
    void setMerge(bool m)
    {
        merge=m;
    }

    void setHitShape(int s,bool b=true)
    {
        shapes[s].hit=b;
    }
    const Shape* getShape(int s)const
    {
        return shapes[s].s.getPointer();
    }

public:
    static const int first,second;

protected:
    virtual Hit _getHit(const Ray& r)const;
    virtual Hit __getHit(const Ray& r)const=0;

protected:
    SmartPointer<const Shape> box;
    struct CSGShapeEntity
    {
        SmartPointer<const Shape> s;
        bool hit;
    } shapes[2];
    bool merge;
};

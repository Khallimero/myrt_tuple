#include "CSGShape.h"

const int CSGShape::first=0;
const int CSGShape::second=1;

CSGShape::CSGShape(SmartPointer<const Shape> s1,SmartPointer<const Shape> s2,bool merge)
    :box(NULL)
{
    this->mark=s1->getMark();
    shapes[CSGShape::first]=(CSGShapeEntity)
    {
        s1,true
    };
    shapes[CSGShape::second]=(CSGShapeEntity)
    {
        s2,true
    };
    setMerge(merge);
}

CSGShape::~CSGShape() {}

Hit CSGShape::_getHit(const Ray& r)const
{
    Hit h=Hit::null;
    if(box.getPointer()==NULL||!box->getHit(r).isNull())
    {
        h=__getHit(r);
        if(merge&&!h.isNull())
            h.setShape(this);
    }
    return h;
}

#include "Mark.h"
#include <stdio.h>

Mark::Mark()
{
    *this=Mark::Ref;
}

Mark::Mark(const Point& o,const Rotation& r)
    :orig(o),rot(r)
{}

Mark::~Mark() {}

Mark Mark::operator+(const Point& p)const
{
    return Mark(getOrig()+p,getRot());
}

Point Mark::fromRef(const Point& p)const
{
    return Point(p-getOrig()).centerRotateInv(Mark::Ref.getOrig(),getRot());
}

Point Mark::toRef(const Point& p)const
{
    return p.centerRotate(Mark::Ref.getOrig(),getRot())+getOrig();
}

Vector Mark::fromRef(const Vector& v)const
{
    return Point(v).centerRotateInv(Mark::Ref.getOrig(),getRot());
}

Vector Mark::toRef(const Vector& v)const
{
    return Point(v).centerRotate(Mark::Ref.getOrig(),getRot());
}

Ray Mark::fromRef(const Ray& r)const
{
    return Ray(fromRef(r.getPoint()),fromRef(r.getVector()));
}

Ray Mark::toRef(const Ray& r)const
{
    return Ray(toRef(r.getPoint()),toRef(r.getVector()));
}

Mark Mark::Ref=Mark(Point::null,Rotation::null);

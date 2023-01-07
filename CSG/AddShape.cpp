#include "AddShape.h"

AddShape::AddShape(SmartPointer<const Shape> s1,SmartPointer<const Shape> s2,bool merge)
    :CSGShape(s1,s2,merge) {}

AddShape::~AddShape() {}

bool AddShape::isInside(const Point& p,double e)const
{
    return (shapes[CSGShape::first].s->isInside(p,e))||(shapes[CSGShape::second].s->isInside(p,e));
}

Hit AddShape::__getHit(const Ray& r)const
{
    Ray rr=r;

    int n=0;
    do
    {
        Hit h1=shapes[CSGShape::first].s->getHit(rr);
        Hit h2=shapes[CSGShape::second].s->getHit(rr);

        double d1=(h1.isNull()?-1:rr.getPoint().dist(h1.getPoint()));
        double d2=(h2.isNull()?-1:rr.getPoint().dist(h2.getPoint()));

        Hit h=Hit::null;
        if((!(h1.isNull()))&&(h2.isNull()||d1<d2))
        {
            if(shapes[CSGShape::first].hit&&!(shapes[CSGShape::second].s->isInside(h1.getPoint())))return h1;
            h=h1;
        }
        if((!(h2.isNull()))&&(h1.isNull()||d2<d1))
        {
            if(shapes[CSGShape::second].hit&&!(shapes[CSGShape::first].s->isInside(h2.getPoint())))return h2;
            h=h2;
        }
        if(h.isNull()||h1.isNull()||h2.isNull())return Hit::null;

        rr=Ray(Point(h.getPoint()+(rr.getVector().norm()*EPSILON)),rr.getVector());
    }
    while(++n<MAX_REC);

    return Hit::null;
}

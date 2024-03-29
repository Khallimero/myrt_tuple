#include "InterShape.h"

InterShape::InterShape(SmartPointer<const Shape> s1,SmartPointer<const Shape> s2,bool merge)
    :CSGShape(s1,s2,merge) {}

InterShape::~InterShape() {}

bool InterShape::isInside(const Point& p,double e)const
{
    return (shapes[CSGShape::first].s->isInside(p,e))&&(shapes[CSGShape::second].s->isInside(p,e));
}

Hit InterShape::__getHit(const Ray& r)const
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
            if(shapes[CSGShape::first].hit&&shapes[CSGShape::second].s->isInside(h1.getPoint()))return h1;
            else h=h1;
        }
        if((!(h2.isNull()))&&(h1.isNull()||d2<d1))
        {
            if(shapes[CSGShape::second].hit&&shapes[CSGShape::first].s->isInside(h2.getPoint()))return h2;
            else h=h2;
        }
        if(h.isNull()||h1.isNull()||h2.isNull())break;

        rr=Ray(h.getPoint()+(rr.getVector().norm()*EPSILON),rr.getVector());
    }
    while(++n<CSG_MAX_REC);

    return Hit::null;
}

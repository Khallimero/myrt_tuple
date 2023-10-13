#include "ShapeCollectionShape.h"

ShapeCollectionShape::ShapeCollectionShape(const Mark& mk):Shape(mk)
{
}

ShapeCollectionShape::~ShapeCollectionShape() {}

bool ShapeCollectionShape::isInside(const Point& p,double e)const
{
    return shapes.isInside(p,e);
}

bool ShapeCollectionShape::intersect(const Ray& r)const
{
    if(getCarveMap()==NULL)return shapes.intersect(r);
    return Shape::intersect(r);
}

Point ShapeCollectionShape::getRelativePoint(const Hit& h,Mapping mp)const
{
    if(mp==Planar)
    {
        int k=h.getId(),n=shapes._count();
        const Shape* s1=shapes[(n+k-1)%n].getPointer();
        const Shape* s2=shapes[(n+k+1)%n].getPointer();
        const Point& o1=s1->getMark().getOrig();
        const Point& o2=s2->getMark().getOrig();
        const Point& ok=shapes[k]->getMark().getOrig();

        Vector u=o1.getVectorTo(o2).norm();
        Point p=Ray(o1,u).proj(ok);
        double x=ok.getVectorTo(h.getPoint()).angle(ok.getVectorTo(p))/(2.0*M_PI);
        Vector v=u.prodVect(ok.getVectorTo(p));
        if(v.cosAngle(ok.getVectorTo(h.getPoint()))<0.0)x=-x;
        double g=u.prodScal(o1.getVectorTo(h.getPoint()))/o1.dist(o2);
        double y=((double)(k-1)/(double)n)*(1.0-g)+((double)(k+1)/(double)n)*(g);
        double z=ok.dist(h.getPoint());

        return Point(x+0.5,y,z);
    }

    return Shape::getRelativePoint(h,mp);
}

Hit ShapeCollectionShape::_getHit(const Ray& r)const
{
    Hit h=shapes.getHit(r);
    if(!h.isNull())h.setShape(this);

    return h;
}

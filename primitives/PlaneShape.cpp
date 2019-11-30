#include "PlaneShape.h"

PlaneShape::PlaneShape(const Mark& mk)
    :Shape(mk)
{
    setTextureMapping(Planar);
    setBumpMapMapping(Planar);
    setCarveMapMapping(Planar);
}

PlaneShape::~PlaneShape() {}

Point PlaneShape::getRelativePoint(const Hit& h,Mapping mp)const
{
    if(mp==Planar)
    {
        Vector n=getNormal();
        Point x=h.getPoint().axialRotate(n,orig,mark.getRot().getRotationSum());

        Vector u=n.getOrtho();
        Vector v=n.prodVect(u).norm();
        Vector w=orig.getVectorTo(x);
        double a=w.prodScal(u);
        double b=w.prodScal(v);

        return Point(a,b,0);
    }

    return Shape::getRelativePoint(h,mp);
}

bool PlaneShape::isInside(const Point& h,double)const
{
    return orig.getVectorTo(h).prodScal(getNormal())<0;
}

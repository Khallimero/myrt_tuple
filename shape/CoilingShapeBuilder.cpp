#include "CoilingShapeBuilder.h"

CoilingShapeBuilder::CoilingShapeBuilder(double fct,double coil,
        double size,double spin,int steps,double rad)
    :ShapeBuilder<1>(size,spin,steps,rad)
{
    this->fct=fct,this->coil=coil;
}

CoilingShapeBuilder::~CoilingShapeBuilder() {}

Point CoilingShapeBuilder::getPoint(const Tuple<int,1>& i,const ParametricPrimitive<1>* prim)const
{
    Point p1=prim->getPoint(_getAngle(i-Tuple<int,1>::Unit));
    Point p2=prim->getPoint(_getAngle(i));
    Point p3=prim->getPoint(_getAngle(i+Tuple<int,1>::Unit));

    Vector v1=p1.getVectorTo(p3);
    Point p=Ray(p1,v1).proj(p2);
    Vector v2=p.getVectorTo(p2).norm()*fct;

    return Point(p+v2).axialRotate(v1,p,get(_getAngle(i))*coil);
}

double CoilingShapeBuilder::getBoxRad()const
{
    return rad+(fct*size);
}

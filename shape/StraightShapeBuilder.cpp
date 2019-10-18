#include "StraightShapeBuilder.h"

StraightShapeBuilder::StraightShapeBuilder(double size,double spin,int steps,double rad)
    :ShapeBuilder<1>(size,spin,steps,rad) {}

StraightShapeBuilder::~StraightShapeBuilder() {}

Point StraightShapeBuilder::getPoint(const Tuple<int,1>& i,const ParametricPrimitive<1>* prim)const
{
    return prim->getPoint(_getAngle(i));
}

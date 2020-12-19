#include "ParametricSurfacePrimitive.h"

ParametricSurfacePrimitive::ParametricSurfacePrimitive(SmartPointer<const ParametricPrimitive<1>> prim)
{
    this->prim=prim;
}

ParametricSurfacePrimitive::~ParametricSurfacePrimitive() {}

Point ParametricSurfacePrimitive::getPoint(const Tuple<double,2>& t)const
{
    const double u=t.get(IPair::first);

    return prim->getPoint(Tuple<double,1>(u));
}

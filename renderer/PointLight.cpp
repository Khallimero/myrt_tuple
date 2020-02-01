#include "PointLight.h"

PointLight::PointLight(const Point& p,const Color& c,double gl)
    :Light(c,gl),p(p),mitigationDist(1.0),mitigationCoeff(2.0)
{
    this->mitigation=false;
}

PointLight::~PointLight() {}

Vector PointLight::getVectorTo(const Point& p)const
{
    return p.getVectorTo(this->p);
}

Point PointLight::getOrig(const Shape* s,const Point& p)const
{
    return this->p;
}

Ray PointLight::getRay(const Shape* s,const Point& p,const Point& o,const NestedIterator<double,2>* it)const
{
    return Ray(this->p,this->p.getVectorTo(p));
}

double PointLight::dist(const Point& p)const
{
    return this->p.dist(p);
}

const Color PointLight::getColor(const Point& p)const
{
    Color col=c;

    if(!p.isNull()&&this->mitigation)
        col/=pow(dist(p)/this->mitigationDist,this->mitigationCoeff);

    return col;
}

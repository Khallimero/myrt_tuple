#include "Plan.h"

#ifndef PLAN_TAN
#define PLAN_TAN (M_PI/1000.0)
#endif

Plan::Plan(double p,const Point& o,const Mark& mk)
    :PlaneShape(mk)
{
    this->p=p;
    this->orig=o;
    if(!FZERO(p))
        this->orig+=getNormal()*_intersectCoeff(Ray(Mark::Ref.getOrig(),getNormal()));
}

Plan::Plan(const Vector& n,const Mark& mk)
    :PlaneShape(Mark(Point(n),mk.getRot()))
{
    init(mk);
}

Plan::Plan(const Vector& v1,const Vector& v2,const Mark& mk)
    :PlaneShape(Mark(Point(v1.prodVect(v2)),mk.getRot()))
{
    init(mk);
}

Plan::~Plan() {}

bool Plan::intersect(const Ray& r)const
{
    if(getCarveMap()==NULL)return _intersectCoeff(r)>0.0;
    return Shape::intersect(r);
}

void Plan::init(const Mark& mk)
{
    this->p=mk.getOrig().prodScal(mark.getOrig());
    this->orig=mk.getOrig();
}

Hit Plan::_getHit(const Ray& r)const
{
    double l=_intersectCoeff(r);
    if(l>0)return Hit(r,this,Point(r.getPoint()+(r.getVector()*l)),getNormal());
    return Hit::null;
}

double Plan::_intersectCoeff(const Ray& r)const
{
    if(fabs(r.getVector().angle(getNormal()))<PLAN_TAN)return 0;
    return (mark.getOrig().prodScal(r.getPoint())-p)
           /r.getVector().prodScal(getNormal());
}

Vector Plan::getNormal()const
{
    return mark.getOrig().getVectorTo(Mark::Ref.getOrig());
}

#pragma once

#include "Light.h"

class PointLight:public Light
{
public:
    PointLight(const Point& p,const Color& c);
    virtual ~PointLight();

public:
    virtual Vector getVectorTo(const Point& p)const;
    virtual Point getOrig(const Shape* s,const Point& p)const;
    virtual Ray getRay(const Shape* s,const Point& p,const Point& o,const NestedIterator<double,2>* it=NULL)const;
    virtual double dist(const Point& p)const;

    virtual double getMitigation(double d)const;

public:
    void setMitigation(double dist,double coeff=2.0)
    {
        this->mitigation=true;
        this->mitigationDist=dist;
        this->mitigationCoeff=coeff;
    }
    double getMitigationDist()const
    {
        return this->mitigationDist;
    }
    double getMitigationCoeff()const
    {
        return this->mitigationCoeff;
    }

protected:
    Point p;
    bool mitigation;
    double mitigationDist;
    double mitigationCoeff;
};

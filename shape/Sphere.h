#pragma once

#include "Ellipse.h"
#include "RoundedIterator.h"

class Sphere:public Ellipse
{
public:
    Sphere(double r,const Mark& mk=Mark::Ref);
    virtual ~Sphere();

public:
    virtual RoundedIterator* getIterator(int ph)const;
    virtual Point getPoint(const Point& p,const NestedIterator<double,2>* it)const;
    virtual Point getPoint(const Point& p,const IteratorElement<double>& i,const IteratorElement<double>& j)const;

public:
    virtual Point getRelativePoint(const Hit& h,Mapping mp=Undefined)const;
};

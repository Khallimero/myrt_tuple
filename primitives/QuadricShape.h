#pragma once

#include "PolyShape.h"

#define QUADRICSHAPE_POLY_ORDER 2

class QuadricShape:public PolyShape<QUADRICSHAPE_POLY_ORDER>
{
protected:
    QuadricShape(const Treble<double>& c,double r,double size,const Mark& mk=Mark::Ref);

public:
    virtual ~QuadricShape();

public:
    virtual bool isInside(const Point& h,double e=0.0)const;
    virtual Point getRelativePoint(const Hit& h,Mapping mp=Undefined)const;

public:
    void setRadius(double r)
    {
        radius=r;
    }
    double getRadius()const
    {
        return radius;
    }

protected:
    void _init()
    {
        static struct quadricshape_unary_op:public inplace_unary_op<double>
        {
            void operator()(double& c)const
            {
                if(!FZERO(c))c=1.0/SQ(c);
            }
        } _quadricshape_unary_op;
        this->coeff.tamper(_quadricshape_unary_op);
    }

protected:
    virtual Hit _getHit(const Ray& r)const;
    virtual TPoly<QUADRICSHAPE_POLY_ORDER> _getPoly(const Point& p,const Vector& v)const;

protected:
    Treble<double> coeff;
    double radius;
};

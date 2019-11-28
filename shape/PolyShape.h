#pragma once

#include "Shape.h"
#include "Poly.h"

template <int ORDER> class PolyShape:public Shape
{
public:
    PolyShape(double size,const Mark& mk=Mark::Ref)
        :Shape(mk)
    {
        this->size=size;
    }

    virtual ~PolyShape() {}

public:
    virtual Point getRelativePoint(const Hit& h,Mapping mp=Volumic)const
    {
        return Shape::getRelativePoint(h,mp)/size;
    }

    virtual bool isInside(const Point& h,double e=0.0)const
    {
        Ray rm=mark.fromRef(Ray(h,Vector(mark.getOrig().getVectorTo(h).norm()/size)));
        PolyRoot r=_getPoly(rm).solve();
        int n=0;
        for(int i=0; i<r.getNbRoots(); i++)
            if(r[i]>-e)n++;

        return (n%2)!=0;
    }

    virtual bool intersect(const Ray& r)const
    {
        if(getCarveMap()==NULL)
        {
            Ray rm=mark.fromRef(r);
            return _intersectCoeff(rm)>0;
        }
        return Shape::intersect(r);
    }

protected:
    virtual TPoly<ORDER> _getPoly(const Point& p,const Vector& v)const=0;

    virtual TPoly<ORDER> _getPoly(const Ray& r)const
    {
        const Point p=r.getPoint()/size;
        const Vector& v=r.getVector();

        return _getPoly(p,v);
    }

    virtual double _intersectCoeff(const Ray& r)const
    {
        return _getPoly(r).solve(false).getFirstRoot()*size;
    }

    template <int SIZE> struct poly_binary_sum:public binary_fct< double,TPoly<2> >
    {
        poly_binary_sum(double f)
        {
            (**this)[0]=f;
        }
        virtual void operator()(const double& d1,const double& d2)
        {
            TPoly<1>::initFromCoeffList(&poly[idx],d1,d2);
            sqPoly[idx]=poly[idx].sq();
            (**this)+=sqPoly[idx];
            ++idx;
        }

        TPoly<1> poly[SIZE];
        TPoly<2> sqPoly[SIZE];

    private:
        tuple_idx<SIZE> idx;
    };

protected:
    double size;
};

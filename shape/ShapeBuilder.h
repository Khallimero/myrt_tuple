#pragma once

#include "ParametricPrimitive.h"
#include "NestedIterator.h"
#include "Shape.h"
#include "Point.h"
#include "Mark.h"

template <int N> class ParametricPrimitive;

enum ShapeComponentType
{
    Spherical,
    SubSpherical,
    Cylindrical
};

template <int N> class TShapeBuilder
{
public:
    TShapeBuilder(double size,double spin,const Tuple<int,N>& steps,double rad=1);
    virtual ~TShapeBuilder();

public:
    double getSize()const
    {
        return size;
    }
    double getSpin()const
    {
        return spin;
    }
    double getRad()const
    {
        return rad;
    }
    virtual double getBoxRad()const
    {
        return rad;
    }

public:
    virtual Point getPoint(const Tuple<int,N>& i,const ParametricPrimitive<N> *prim)const=0;

protected:
    virtual Point _getPoint(const Tuple<int,N>& i,const ParametricPrimitive<N> *prim,const Mark& mk=Mark::Ref)const;
    virtual Point _getPoint(const Point& p,const Mark& mk=Mark::Ref)const;
    Tuple<double,N> _getAngle(const Tuple<int,N>& t)const;
    NestedIterator<int,N> _getIterator()const;

protected:
    double size,spin;
    Tuple<int,N> steps;
    double rad;
};

template <int N> class ShapeBuilder:public TShapeBuilder<N> {};

template <> class ShapeBuilder<1>:public TShapeBuilder<1>
{
public:
    ShapeBuilder(double size,double spin,int steps,double rad=1)
        :TShapeBuilder<1>(size,spin,Tuple<int,1>(steps),rad) {}
    virtual ~ShapeBuilder() {}

public:
    Shape* getBoxedInstance(const Mark& mk,
                            const ParametricPrimitive<1> *prim,
                            ShapeComponentType sType,
                            double radius=-1)const;
    Shape* getInstance(const Mark& mk,
                       const ParametricPrimitive<1> *prim,
                       ShapeComponentType sType)const;
};

template <> class ShapeBuilder<2>:public TShapeBuilder<2>
{
public:
    ShapeBuilder(double size,double spin,const Tuple<int,2>& steps)
        :TShapeBuilder<2>(size,spin,steps) {}
    virtual ~ShapeBuilder() {}

public:
    Shape* getInstance(const Mark& mk,
                       const ParametricPrimitive<1> *prim,
                       double rad,
                       bool smoothNormal=true)const;

    Shape* getInstance(const Mark& mk,
                       const ParametricPrimitive<2> *prim,
                       bool smoothNormal=true)const;

    Shape* getInstance(const Mark& mk,
                       const ParametricPrimitive<2> *prim,
                       double rad)const;

public:
    virtual Point getPoint(const Tuple<int,2>& i,const ParametricPrimitive<2>* prim)const;

protected:
    virtual Point __getPoint(const Tuple<int,2>& i,const ParametricPrimitive<1> *prim,const Mark& mk,double rad)const;
};

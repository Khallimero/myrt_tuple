#pragma once

#include "Tuple.h"
#include "Shape.h"
#include "ShapeBuilder.h"
#include "SmartPointer.h"

template <int N> class TShapeBuilder;

template <int N> class ParametricPrimitive
{
public:
    ParametricPrimitive() {}
    virtual ~ParametricPrimitive() {}

public:
    virtual Point getPoint(const Tuple<double,N>& t)const=0;
    virtual double getRad(const Tuple<double,N>&)const
    {
        return 1.0;
    }

public:
    virtual SmartPointer<Shape> getBox(const TShapeBuilder<N>* builder,const Mark& mk)const
    {
        return SmartPointer<Shape>(NULL);
    }
};

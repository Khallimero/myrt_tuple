#pragma once

#include "ParametricPrimitive.h"

template <int N> class CustomParametricPrimitive:public ParametricPrimitive<N>
{
public:
    CustomParametricPrimitive(Point (*pFct)(const Tuple<double,N>&),double (*rFct)(const Tuple<double,N>&))
    {
        this->pFct=pFct,this->rFct=rFct;
    }
    virtual ~CustomParametricPrimitive() {}

public:
    virtual Point getPoint(const Tuple<double,N>& t)const
    {
        return pFct(t);
    }
    virtual double getRad(const Tuple<double,N>& t)const
    {
        return rFct(t);
    }

protected:
    Point (*pFct)(const Tuple<double,N>&);
    double (*rFct)(const Tuple<double,N>&);
};

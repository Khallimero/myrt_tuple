#pragma once

#include "Arithmetic.h"

class Geometric:public Arithmetic< Treble<double> >
{
public:
    Geometric();
    Geometric(double x,double y,double z);
    Geometric(const Tuple<double,TREBLE_SIZE>& that);
    virtual ~Geometric();

public:
    double prodScal(const Geometric& that)const;

public:
    double getX()const
    {
        return this->get(ITreble::first);
    }
    double getY()const
    {
        return this->get(ITreble::second);
    }
    double getZ()const
    {
        return this->get(ITreble::third);
    }
};

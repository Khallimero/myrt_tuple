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
    double prodScal(const Geometric& that)const
    {
        struct treble_binary_fct:public binary_fct<double,double>
        {
            virtual void operator()(const double& t1,const double& t2)
            {
                (**this)+=t1*t2;
            }
        } _binary_fct;
        return this->tamper(that,_binary_fct);
    }

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

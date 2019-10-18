#pragma once

#include "Treble.h"

template <typename T> class Arithmetic:public T
{
public:
    Arithmetic() {}
    Arithmetic(const T& that):T(that) {}
    virtual ~Arithmetic() {}

public:
    double sumSq()const
    {
        struct treble_unary_fct:public unary_fct<double,double>
        {
            virtual void operator()(const double& t)
            {
                (**this)+=SQ(t);
            }
        } _unary_fct;
        return this->tamper(_unary_fct);
    }

    Arithmetic<T> sq()const
    {
        static struct arithmetic_unary_op:public unary_op<double,double>
        {
            virtual double operator()(const volatile double& t)const
            {
                return SQ(t);
            }
        } _unary_op;
        return Arithmetic<T>(this->tamper(_unary_op));
    }

    double sqrt()const
    {
        return ::sqrt(this->sumSq());
    }
};

template <typename T> inline double sumSq(const Arithmetic<T>& that)
{
    return that.sumSq();
}

template <typename T> inline Arithmetic<T> sq(const Arithmetic<T>& that)
{
    return that.sq();
}

template <typename T> inline double sqrt(const Arithmetic<T>& that)
{
    return that.sqrt();
}

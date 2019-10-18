#pragma once

#include "Arithmetic.h"
#include "Pair.h"

#include <math.h>

class Complex;

Complex csqrt(double);
Complex csqrt(const Complex&);
double abs(const Complex&);
double arg(const Complex&);
Complex inv(const Complex&);
Complex conj(const Complex&);
Complex pow(const Complex&,const Complex&);
Complex exp(const Complex&);
Complex log(const Complex&);

Complex sin(const Complex&);
Complex cos(const Complex&);
Complex tan(const Complex&);
Complex sinh(const Complex&);
Complex cosh(const Complex&);
Complex tanh(const Complex&);

class Complex:public Arithmetic< Pair<double> >
{
public:
    Complex();
    Complex(const Tuple<double,PAIR_SIZE> &that);
    Complex(double r,double i=0.0);
    virtual ~Complex();

public:
    Complex operator*(const Complex& that)const
    {
        return Complex(this->getReal()*that.getReal()-this->getImg()*that.getImg(),
                       this->getReal()*that.getImg()+this->getImg()*that.getReal());
    }

    Complex operator*(const double d)const
    {
        return ((const Arithmetic< Pair<double> >*)this)->operator*(d);
    }

    Complex operator/(const Complex& that)const
    {
        return (*this)*that.conj()*(1.0/SQ(that.abs()));
    }

    Complex operator/(const double d)const
    {
        return ((const Arithmetic< Pair<double> >*)this)->operator/(d);
    }

public:
    double getReal()const
    {
        return this->get(IPair::first);
    }
    double getImg()const
    {
        return this->get(IPair::second);
    }
    bool isReal()const
    {
        return FZERO(getImg());
    }

public:
    double abs()const
    {
        return sqrt();
    }

    Complex inv()const
    {
        return conj()/sumSq();
    }

    Complex conj()const
    {
        return Complex(getReal(),-getImg());
    }

    Complex csqrt()const
    {
        return pow(Complex(0.5));
    }

    Complex exp()const
    {
        return Complex(::exp(getReal())*cos(getImg()),::exp(getReal())*sin(getImg()));
    }

    Complex log()const
    {
        return Complex(::log(abs()),arg());
    }

    Complex pow(const Complex& c)const
    {
        return ::exp(c*log());
    }

    double arg()const
    {
        return getReal()<0?getImg()<0?
               atan(getImg()/getReal())-M_PI:
               M_PI-atan(-getImg()/getReal()):
               atan(getImg()/getReal());
    }

public:
    static const Complex null;
    static const Complex one;
    static const Complex i;
};

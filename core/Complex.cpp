#include "Complex.h"

const Complex Complex::null=Complex(0,0);
const Complex Complex::one=Complex(1,0);
const Complex Complex::i=Complex(0,1);

Complex::Complex() {}

Complex::Complex(const Tuple<double,PAIR_SIZE> &that)
    :Arithmetic< Pair<double> >(Pair<double>(that)) {}

Complex::Complex(double x,double y)
    :Arithmetic< Pair<double> >(Pair<double>(x,y)) {}

Complex::~Complex() {}

Complex csqrt(double d)
{
    double r=sqrt(fabs(d));
    return d<0.0?Complex(0,r):Complex(r);
}

Complex csqrt(const Complex& c)
{
    return c.csqrt();
}

double abs(const Complex& c)
{
    return c.abs();
}

double arg(const Complex& c)
{
    return c.arg();
}

Complex inv(const Complex& c)
{
    return c.inv();
}

Complex conj(const Complex& c)
{
    return c.conj();
}

Complex pow(const Complex& a,const Complex& b)
{
    return a.pow(b);
}

Complex exp(const Complex& c)
{
    return c.exp();
}

Complex log(const Complex& c)
{
    return c.log();
}

Complex sin(const Complex& c)
{
    return (exp(Complex::i*c)-exp(-Complex::i*c))/(Complex::i*2.0);
}

Complex cos(const Complex& c)
{
    return (exp(Complex::i*c)+exp(-Complex::i*c))/2.0;
}

Complex tan(const Complex& c)
{
    return sin(c)/cos(c);
}

Complex sinh(const Complex& c)
{
    return (exp(c)-exp(-c))/2.0;
}

Complex cosh(const Complex& c)
{
    return (exp(c)+exp(-c))/2.0;
}

Complex tanh(const Complex& c)
{
    return (exp(c*2.0)-Complex::one)/(exp(c*2.0)+Complex::one);
}

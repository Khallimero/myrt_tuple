#pragma once

#include "PolyRoot.h"
#include "Complex.h"

#include <stdarg.h>

class Poly
{
public:
    virtual ~Poly();

public:
    PolyRoot solve(bool collect=true)const;
    double eval(double x)const;
    Complex eval(const Complex& c)const;

    virtual int getOrder()const=0;
    virtual double& operator[](int)=0;
    virtual double get(int)const=0;
    virtual void set(double,int)=0;
    virtual const double* getCoeffs()const=0;

    virtual Poly* normalize()const=0;

    void _normalize();

protected:
    PolyRoot solveConst(bool collect=true)const;
    PolyRoot solveLinear(bool collect=true)const;
    PolyRoot solveQuadratic(bool collect=true)const;
    PolyRoot solveCubic(bool collect=true)const;
    PolyRoot solveQuartic(bool collect=true)const;
    PolyRoot solveGeneric(bool collect=true)const;

    int _getOrder()const;

private:
    double maxValue(const Complex* c,int n)const;
    static Complex WeierNull(const Complex* z,int n,int k);
};

template <int ORDER> class TPoly:public Poly
{
public:
    TPoly(double c=0.0)
    {
        coeffs[0]=c;
        for(int i=1; i<=ORDER; i++)
            coeffs[i]=0.0;
    }
    TPoly(const Poly& that)
    {
        *this=that;
    }
    TPoly(const Poly& that,double f)
    {
        for(int i=0; i<=ORDER; i++)
            coeffs[i]=that.get(i)*f;
    }
    TPoly(const Poly* p1,const Poly* p2,void (TPoly::*f)(const Poly*,const Poly*))
    {
        (this->*f)(p1,p2);
    }
    TPoly& operator=(const Poly& that)
    {
        for(int i=0; i<=ORDER; i++)
            coeffs[i]=that.get(i);
        return *this;
    }
    virtual ~TPoly() {}

public:
    int getOrder()const
    {
        return ORDER;
    }
    double& operator[](int n)
    {
        return coeffs[n];
    }
    double get(int n)const
    {
        return n>ORDER?0.0:coeffs[n];
    }
    void set(double c,int n)
    {
        coeffs[n]=c;
    }
    const double* getCoeffs()const
    {
        return coeffs;
    }

    static void initFromCoeffList(Poly* p,...)
    {
        va_list vl;
        va_start(vl,p);
        for(int i=0; i<=ORDER; i++)
            (*p)[i]=va_arg(vl,double);
        va_end(vl);
    }

public:
    TPoly operator+(const TPoly& that)const
    {
        return TPoly(this,&that,&TPoly::setAdd);
    }

    TPoly& operator+=(const TPoly& that)
    {
        setAdd(this,&that);
        return *this;
    }

    TPoly operator-(const TPoly& that)const
    {
        TPoly p=-that;
        return TPoly(this,&p,&TPoly::setAdd);
    }

    TPoly& operator-=(const TPoly& that)
    {
        TPoly p=-that;
        setAdd(this,&p);
        return *this;
    }

    TPoly operator*(const TPoly& that)const
    {
        return TPoly(this,&that,&TPoly::setMul);
    }

    TPoly operator*(const double f)const
    {
        return TPoly(*this,f);
    }

    TPoly operator/(const double f)const
    {
        return (*this)*(1.0/f);
    }

    TPoly operator-()const
    {
        return (*this)*-1.0;
    }

    TPoly<ORDER*2> sq()const
    {
        return TPoly<ORDER*2>(this,this,&TPoly<ORDER*2>::setMul);
    }

    TPoly<ORDER-1> diff()const
    {
        TPoly<ORDER-1> p;
        for(int i=0; i<ORDER; i++)
            p[i]=this->get(i+1)*((double)i+1);
        return p;
    }

public:
    void setAdd(const Poly* p1,const Poly* p2)
    {
        for(int i=0; i<=ORDER; i++)
            coeffs[i]=p1->get(i)+p2->get(i);
    }

    void setMul(const Poly* p1,const Poly* p2)
    {
        for(int i=0; i<=ORDER; i++)
        {
            coeffs[i]=0;
            for(int j=0; j<=i; j++)
                coeffs[i]+=p1->get(j)*p2->get(i-j);
        }
    }

public:
    Poly* normalize()const
    {
        if(ORDER>0&&FZERO(coeffs[ORDER]))
        {
            static const int order=(ORDER>0?ORDER-1:0);
            return TPoly<order>(*this).normalize();
        }
        Poly* p=new TPoly<ORDER>(*this);
        p->_normalize();
        return p;
    }

    static TPoly<ORDER> normalize(const Poly* that)
    {
        TPoly<ORDER> p(*that);
        p._normalize();
        return p;
    }

protected:
    double coeffs[ORDER+1];
};

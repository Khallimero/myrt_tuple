#include "Poly.h"
#include "SmartPointer.h"

#include <math.h>

#define POLY_EPS 1e-6
#define POLY_IT 1000

Poly::~Poly() {}

double Poly::eval(double x)const
{
    double v=0.0,p=1.0;
    for(int i=0; i<=getOrder(); i++,p*=x)
        v+=get(i)*p;
    return v;
}

Complex Poly::eval(const Complex& c)const
{
    Complex r=get(getOrder());
    for(int i=getOrder()-1; i>=0; i--)
        r=c*r+Complex(get(i));
    return r;
}

int Poly::_getOrder()const
{
    int n=getOrder();
    while(n>0&&FZERO(get(n)))n--;
    return n;
}

void Poly::_normalize()
{
    double c=get(getOrder());
    if(c!=0.0)
        for(int i=0; i<=getOrder(); i++)
            set(get(i)/c,i);
}

PolyRoot Poly::solve(bool collect)const
{
    switch(_getOrder())
    {
    case 0:
        return solveConst(collect);
    case 1:
        return solveLinear(collect);
    case 2:
        return solveQuadratic(collect);
    case 3:
        return solveCubic(collect);
    case 4:
        return solveQuartic(collect);
    };

    return solveGeneric(collect);
}

PolyRoot Poly::solveConst(bool collect)const
{
    return PolyRoot(collect);
}

PolyRoot Poly::solveLinear(bool collect)const
{
    PolyRoot r(collect);
    r.addRoot(-get(0)/get(1));
    return r;
}

PolyRoot Poly::solveQuadratic(bool collect)const
{
    PolyRoot r(collect);
    double d=SQ(get(1))-4.0*get(0)*get(2);
    if(d==0.0)
    {
        r.addRoot(-get(1)/(2.0*get(2)));
    }
    else if(d>0)
    {
        r.addRoot((-sqrt(d)-get(1))/(2.0*get(2)));
        r.addRoot((sqrt(d)-get(1))/(2.0*get(2)));
    }

    return r;
}

PolyRoot Poly::solveCubic(bool collect)const
{
    PolyRoot r(collect);
    TPoly<3> p=TPoly<3>::normalize(this);

    double A2=SQ(p.get(2));
    double Q=(A2-3.0*p.get(1))/9.0;
    double R=(p.get(2)*(A2-4.5*p.get(1))+13.5*p.get(0))/27.0;
    double Q3=CB(Q);
    double R2=SQ(R);
    double d=Q3-R2;
    double an=p.get(2)/3.0;

    if(d>=0)
    {
        d=R/sqrt(Q3);
        double t=acos(d)/3.0;
        double sQ=-2.0*sqrt(Q);

        r.addRoot(sQ*cos(t)-an);
        r.addRoot(sQ*cos(t+2.0*M_PI/3.0)-an);
        r.addRoot(sQ*cos(t+4.0*M_PI/3.0)-an);
    }
    else
    {
        double sQ=cbrt(sqrt(R2-Q3)+fabs(R));
        if(R<0)r.addRoot((sQ+Q/sQ)-an);
        else r.addRoot(-(sQ+Q/sQ)-an);
    }

    return r;
}

PolyRoot Poly::solveQuartic(bool collect)const
{
    PolyRoot rt(collect);
    TPoly<4> pl=TPoly<4>::normalize(this);

    double c12=SQ(pl.get(3));
    double p=-3.0*c12/8.0+pl.get(2);
    double q=c12*pl.get(3)/8.0-pl.get(3)*pl.get(2)/2.0+pl.get(1);
    double r=-3.0*SQ(c12)/256.0+c12*pl.get(2)/16.0-pl.get(3)*pl.get(1)/4.0+pl.get(0);

    TPoly<3> cb;
    cb[0]=r*p/2.0-SQ(q)/8.0,cb[1]=-r,cb[2]=-p/2.0,cb[3]=1.0;
    double z=cb.solveCubic(false).getFirstRoot();
    double d1=2.0*z-p;
    double d2=0.0;

    if(d1==0.0)
    {
        d2=SQ(z)-r;
        if(d2<0.0)return rt;
        d2=sqrt(d2);
    }
    else if(d1<0.0)
    {
        return rt;
    }
    else
    {
        d1=sqrt(d1);
        d2=(q/2.0)/d1;
    }

    double q1=SQ(d1);
    double q2=-pl.get(3)/4.0;

    p=q1-4.0*(z-d2);
    if(p==0.0)rt.addRoot(-d1/2.0-q2);
    else if(p>0)
    {
        p=sqrt(p);
        rt.addRoot(-(d1+p)/2.0+q2);
        rt.addRoot(-(d1-p)/2.0+q2);
    }

    p=q1-4.0*(z+d2);
    if(p==0.0)rt.addRoot(d1/2.0-q2);
    else if(p>0)
    {
        p=sqrt(p);
        rt.addRoot((d1+p)/2.0+q2);
        rt.addRoot((d1-p)/2.0+q2);
    }

    return rt;
}

PolyRoot Poly::solveGeneric(bool collect)const
{
    SmartPointer<Poly> pl(this->normalize());
    int ord=pl->getOrder();
    Complex* z=new Complex[ord]();
    Complex* w=new Complex[ord]();

    for(int i=0; i<ord; i++)
        z[i]=exp(Complex::i*(2.0*M_PI*(double)i/(double)ord));

    for(int i=0; i<POLY_IT&&pl->maxValue(z,ord)>POLY_EPS; i++)
        for(int j=0; j<10; j++)
        {
            for(int k=0; k<ord; k++)
                w[k]=pl->eval(z[k])/Poly::WeierNull(z,ord,k);
            for(int k=0; k<ord; k++)
                z[k]-=w[k];
        }

    PolyRoot rt(collect);
    for(int i=0; i<ord; i++)
        if(z[i].isReal())
            rt.addRoot(z[i].getReal());

    delete[] z;
    delete[] w;

    return rt;
}

double Poly::maxValue(const Complex* c,int n)const
{
    double m=0;
    for(int i=0; i<n; i++)
        m=MAX(m,abs(eval(c[i])));
    return m;
}

Complex Poly::WeierNull(const Complex* z,int n,int k)
{
    Complex r=Complex::one;
    for(int i=0; i<n; i++)
        if(i!=k)r=r*(z[k]-z[i]);
    return r;
}

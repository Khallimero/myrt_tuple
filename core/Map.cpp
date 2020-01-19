#include "Map.h"
#include "Color.h"
#include "Vector.h"
#include "Pair.h"

#include <math.h>

template <typename T> T bilinearFilter(T** tab,int w,int h,double x0,double y0)
{
    class LerpPair:public Pair<double>
    {
    public:
        LerpPair(double _d0)
        {
            this->d0=fabs(_d0);
            (*this)[IPair::first]=floor(d0);
            (*this)[IPair::second]=ceil(d0);
        }

        operator double()
        {
            return d0;
        }

    protected:
        double d0;
    };

    struct filter:public binary_fct<double,T>
    {
        filter(T** tab,int w,int h,double x0,double y0)
        {
            this->tab=tab;
            this->w=w,this->h=h;
            this->x0=x0,this->y0=y0;
        }
        virtual void operator()(const double& x,const double& y)
        {
            (**this)+=T(tab[(int)x%w][(int)y%h])*((1.0-fabs(x0-x))*(1.0-fabs(y0-y)));
        }
        T** tab;
        int w,h;
        double x0,y0;
    };

    LerpPair x(x0),y(y0);
    filter flt(tab,w,h,x,y);

    TUPLE_IDX(i,PAIR_SIZE)
    {
        TUPLE_IDX(j,PAIR_SIZE)
        flt(x[i],y[j]);
    }
    return flt;
}

template Vector bilinearFilter<Vector>(Vector**,int,int,double,double);
template Color bilinearFilter<Color>(Color**,int,int,double,double);

template class Map<Color>;
template class Map<Vector>;

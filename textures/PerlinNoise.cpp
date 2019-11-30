#include "PerlinNoise.h"
#include "Pair.h"
#include "Treble.h"

#include <math.h>

PerlinNoise::PerlinNoise()
{
    this->p=new int[B+B+2];
    this->g3=new Vector[B+B+2]();

    for(int i=0; i<B; i++)
    {
        p[i]=i;
        TUPLE_IDX(j,TREBLE_SIZE)
        g3[i][j]=(double)((rand()%(B+B))-B)/B;
        g3[i]=g3[i].norm();
    }

    for(int i=B-1; i>0; i--)
    {
        int j=rand()%B;
        SWAP(p[i],p[j]);
    }

    for(int i=0; i<B+2; i++)
    {
        p[B+i]=p[i];
        TUPLE_IDX(j,TREBLE_SIZE)
        g3[B+i][j]=g3[i][j];
    }
}

PerlinNoise::~PerlinNoise()
{
    delete[] p;
    delete[] g3;
}

double PerlinNoise::noise(const Point& x)const
{
    Pair<int> bp[PAIR_SIZE];
    Treble<int> bt[PAIR_SIZE];
    Treble<double> r[PAIR_SIZE];

    TUPLE_IDX(idx,TREBLE_SIZE)
    {
        double t=x(idx)+N;
        bt[IPair::first][idx]=((int)t)&BM;
        bt[IPair::second][idx]=(bt[IPair::first][idx]+1)&BM;
        r[IPair::first][idx]=t-(int)t;
        r[IPair::second][idx]=r[IPair::first][idx]-1.0;
    }

    TUPLE_IDX(idx0,PAIR_SIZE)
    {
        TUPLE_IDX(idx1,PAIR_SIZE)
        {
            bp[idx0][idx1]=p[p[bt[idx0][ITreble::first]]+
                             bt[idx1][ITreble::second]];
        }
    }

    static struct tuple_unary_op:public unary_op<double,double>
    {
        virtual double operator()(const volatile double& t)const
        {
            return SQ(t)*(3.0-2.0*t);
        }
    } _unary_op;
    Treble<double> s(Tuple<double,TREBLE_SIZE>(r[IPair::first],_unary_op));

    tuple_idx<PAIR_SIZE> idx[TREBLE_SIZE];
    return _noise(idx,bp,bt,r,s);
}

double PerlinNoise::_noise(tuple_idx<PAIR_SIZE>* idx,
                           Pair<int>* bp,
                           Treble<int>* bt,
                           Treble<double>* r,
                           Treble<double>& s,
                           int n)const
{
    if(n<=0)
    {
        struct vector_unary_idx_op:public unary_idx_op<double,TREBLE_SIZE>
        {
            vector_unary_idx_op(tuple_idx<PAIR_SIZE>* idx,Treble<double>* r)
            {
                this->idx=idx,this->r=r;
            }
            virtual double operator()(const tuple_idx<TREBLE_SIZE>& i)const
            {
                return r[idx[i]][i];
            }
            tuple_idx<PAIR_SIZE>* idx;
            Treble<double>* r;
        };

        const Vector v=Vector(vector_unary_idx_op(idx,r));
        return g3[bp[idx[ITreble::first]][idx[ITreble::second]]+
                  bt[idx[ITreble::third]][ITreble::third]].prodScal(v);
    }

    Pair<double> pd;
    idx[--n].reset();
    do
    {
        pd[idx[n]]=_noise(idx,bp,bt,r,s,n);
    }
    while((++idx[n]).isValid());

    return PerlinNoise::lerp(s[n],pd);
}

double PerlinNoise::perlinNoise(const Point& x,double alpha,double beta,int n)const
{
    return _perlinNoise(x,alpha,beta,n,false);
}

double PerlinNoise::perlinNoiseAbs(const Point& x,double alpha,double beta,int n)const
{
    return _perlinNoise(x,alpha,beta,n,true);
}

double PerlinNoise::_perlinNoise(const Point& x,double alpha,double beta,int n,bool flg)const
{
    double val,sum=0.0;
    double scale=1.0;
    Point p=x;

    for(int i=0; i<n; i++)
    {
        val=noise(p);
        if(flg)val=fabs(val);
        sum+=val/scale;
        scale*=alpha;
        p*=beta;
    }
    return(sum);
}

double PerlinNoise::lerp(double t,const Pair<double>& p)
{
    return p(0)+t*(p(1)-p(0));
}

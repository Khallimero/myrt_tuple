#pragma once

#include "../myrt.h"

#include <math.h>
#include <stdlib.h>

//#define MIN(a,b) ({typeof(a) _a=a;typeof(b) _b=b;_a>_b?_b:_a;})
//#define MAX(a,b) ({typeof(a) _a=a;typeof(b) _b=b;_a<_b?_b:_a;})
//#define SQ(x) ({typeof(x) _x=x;_x*_x;})
//#define CB(x) ({typeof(x) _x=x;_x*_x*_x;})
//#define SWAP(a,b) ({typeof(a) _a=a;typeof(b) _b=b;a=_b,b=_a;})

template<typename T> T MIN(T a,T b)
{
    T _a=a;
    T _b=b;
    return _a> _b ? _b:_a;
}

template<typename T> T MAX(T a,T b)
{
    T _a=a;
    T _b=b;
    return _a< _b ? _b:_a;
}

template<typename T> T SQ(T x)
{
    T _x=x;
    return _x*_x;
}

template<typename T> T CB(T x)
{
    T _x=x;
    return _x*_x*_x;
}

template<typename T> void SWAP(T& a,T& b)
{
    T _a=a;
    T _b=b;
    a=_b,b=_a;
}

#define FCMP(a,b) (fabs((a)-(b))<=0.00000001)
#define FZERO(a) (FCMP(a,0))
#define RND ((double)rand()/RAND_MAX)
#define IRND(x) (rand()%(x))

//#define SIGN(x) (x<0?-1:1)
//#define ZSIGN(x) ({typeof(x) _x=x;x==0?0:x<0?-1:1;})

template<typename T> int SIGN(T x)
{
    return x<0?-1:1;
}

template<typename T> int ZSIGN(T x)
{
    return x==0?0:x<0?-1:1;
}

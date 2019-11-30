#pragma once

#include "Point.h"
#include "Vector.h"
#include "Pair.h"

class PerlinNoise
{
public:
    PerlinNoise();
    virtual ~PerlinNoise();

public:
    double noise(const Point& x)const;
    double perlinNoise(const Point& x,double alpha,double beta,int n)const;
    double perlinNoiseAbs(const Point& x,double alpha,double beta,int n)const;

protected:
    double _perlinNoise(const Point& x,double alpha,double beta,int n,bool flg=true)const;
    double _noise(tuple_idx<PAIR_SIZE>* idx,
                  Pair<int>* bp,
                  Treble<int>* bt,
                  Treble<double>* r,
                  Treble<double>& s,
                  int n=TREBLE_SIZE)const;

    static double lerp(double t,const Pair<double>& p);

protected:
    int *p;
    Vector *g3;

private:
    static const int B=0x100;
    static const int BM=0xff;
    static const int N=0x1000;
    static const int NP=12;
    static const int NM=0xfff;
};

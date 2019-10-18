#pragma once

#include "Tuple.h"

class IPair
{
public:
    static const int first;
    static const int second;
};

template <typename T> class Pair:public Tuple<T,PAIR_SIZE>,public IPair
{
public:
    Pair() {}

    Pair(const T a,const T b)
    {
        this->vTab[IPair::first]=a;
        this->vTab[IPair::second]=b;
    }

    Pair(const Tuple<T,PAIR_SIZE> &that):Tuple<T,PAIR_SIZE>(that) {}
};

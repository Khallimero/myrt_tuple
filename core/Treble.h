#pragma once

#include "Tuple.h"
#include "Pair.h"

class ITreble:public IPair
{
public:
    static const int third;
};

template <typename T> class Treble:public Tuple<T,TREBLE_SIZE>,public ITreble
{
public:
    Treble() {}

    Treble(const T a,const T b,const T c)
    {
        this->vTab[ITreble::first]=a;
        this->vTab[ITreble::second]=b;
        this->vTab[ITreble::third]=c;
    }

    Treble(const Tuple<T,TREBLE_SIZE> &that):Tuple<T,TREBLE_SIZE>(that) {}
};

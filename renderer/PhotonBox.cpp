#include "PhotonBox.h"

#include <math.h>
#include <string.h>

#ifndef PHOTON_BOX_FCT
#define PHOTON_BOX_FCT 2.0
#endif

static struct photonbox_unary_op:public unary_op<double,int>
{
    virtual int operator()(const volatile double& t)const
    {
        return PhotonBox::getInd(t);
    }
} _unary_op;

PhotonBox::PhotonBox(const Treble<int>& that)
    :Treble<int>(that) {}

PhotonBox::PhotonBox(const Point& p)
    :Treble<int>(Tuple<int,TREBLE_SIZE>(p,_unary_op)) {}

PhotonBox::~PhotonBox() {}

int PhotonBox::compareTo(const PhotonBox& that)const
{
    TUPLE_IDX(i,TREBLE_SIZE)
    if(this->get(i)!=that.get(i))
        return this->get(i)-that.get(i);
    return 0;
}

int PhotonBox::getInd(double d)
{
    return (int)floor(d/PHOTON_BOX_FCT);
}

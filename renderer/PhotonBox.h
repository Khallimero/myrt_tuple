#pragma once

#include "Treble.h"
#include "Point.h"

class PhotonBox:protected Treble<int>
{
public:
    PhotonBox(const Treble<int>& that);
    PhotonBox(const Point& p=Point::null);
    virtual ~PhotonBox();

public:
    void dump(int *t)const;
    int compareTo(const PhotonBox& that)const;

public:
    static int getInd(double d);
};

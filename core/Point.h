#pragma once

#include "Geometric.h"
#include "Rotation.h"
#include "Vector.h"

class Point:public Geometric
{
public:
    Point();
    Point(double x,double y,double z);
    Point(const Tuple<double,TREBLE_SIZE>& that);
    virtual ~Point();

public:
    Vector getVectorTo(const Point& that)const;
    Point centerRotate(const Point& c,const Rotation& r)const;
    Point centerRotateInv(const Point& c,const Rotation& r)const;
    Point axialRotate(const Vector& ax,const Point& o,double a)const;
    double dist(const Point& that=Point::null)const;

public:
    static Point barycenter(const Point& p1,const Point& p2,
                            double a=1.0,double b=1.0);
    static Point barycenter(const Point& p1,const Point& p2,const Point& p3,
                            double a=1.0,double b=1.0,double c=1.0);

public:
    static const Point null;
};

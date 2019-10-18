#pragma once

#include "Geometric.h"

class Vector:public Geometric
{
public:
    Vector();
    Vector(double x,double y,double z);
    Vector(const Tuple<double,TREBLE_SIZE>& that);
    virtual ~Vector();

public:
    double length()const;
    Vector norm()const;
    Vector prodVect(const Vector& that)const;
    double cosAngle(const Vector& that)const;
    double angle(const Vector& that)const;
    Vector reflect(const Vector& normal)const;
    Vector refract(const Vector& normal,double n1,double n2)const;
    Vector getOrtho()const;
    Vector projOrthoTo(const Vector& that)const;
    Vector projOn(const Vector& that)const;

public:
    static Vector random();

public:
    static const Vector null;
    static const Vector Unit;
    static const Vector UnitX,UnitY,UnitZ;
    static const TupleRef<Vector> UnitTab[3];
    static const Vector EpsilonX,EpsilonY,EpsilonZ;
    static const TupleRef<Vector> EpsilonTab[3];
};

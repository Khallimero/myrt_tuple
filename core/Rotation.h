#pragma once

#include "Treble.h"
#include "Vector.h"

class Rotation:protected Treble<double>
{
public:
    Rotation();
    Rotation(double x,double y,double z);
    Rotation(const Tuple<double,TREBLE_SIZE>& that);
    Rotation(const Vector& v);
    virtual ~Rotation();

public:
    Rotation norm()const;
    Rotation reverse()const;

public:
    static Rotation random();

public:
    const double getRotationValue(int i)const
    {
        return Tuple<double,TREBLE_SIZE>::get(i);
    }
    const double getRotationSum()const
    {
        return this->sum();
    }

    const Vector& getRotationVectorX()const
    {
        return rotationVector[0];
    }
    const Vector& getRotationVectorY()const
    {
        return rotationVector[1];
    }
    const Vector& getRotationVectorZ()const
    {
        return rotationVector[2];
    }
    const Vector* getRotationVector()const
    {
        return rotationVector;
    }

protected:
    void _initRotationVectors();
    static Vector _getRotationVectorX();
    static Vector _getRotationVectorY(const Vector& vx,double rx);
    static Vector _getRotationVectorZ(const Vector& vx,double rx,const Vector& vy,double ry);

protected:
    Vector rotationVector[3];

public:
    static const Rotation null;
};

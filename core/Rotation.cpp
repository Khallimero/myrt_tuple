#include "Rotation.h"
#include "Point.h"
#include "Mark.h"

#include <math.h>

const Rotation Rotation::null=Rotation();

Rotation::Rotation():Treble<double>()
{
    _initRotationVectors();
}

Rotation::Rotation(double x,double y,double z):Treble<double>(x,y,z)
{
    _initRotationVectors();
}

Rotation::Rotation(const Tuple<double,TREBLE_SIZE>& that):Treble<double>(that)
{
    _initRotationVectors();
}

Rotation::Rotation(const Vector& v)
{
    if(FCMP(fabs(v.cosAngle(Vector::UnitX)),1.))
    {
        *this=Rotation(0,0,M_PI/2.0*SIGN(v.cosAngle(Vector::UnitX)));
    }
    else
    {
        Vector v1=v.projOrthoTo(Vector::UnitX);
        double alpha=v1.angle(Vector::UnitY);
        if(v1.prodVect(Vector::UnitY).prodScal(Vector::UnitX)>0)alpha*=-1.0;
        Vector v2=Vector(Point(Vector::UnitY).axialRotate(Vector::UnitX,Mark::Ref.getOrig(),alpha));
        double beta=v.angle(v2);
        if(v.prodVect(v2).prodScal(Point(Vector::UnitZ).axialRotate(Vector::UnitX,Mark::Ref.getOrig(),alpha))>0)beta*=-1.0;

        *this=Rotation(alpha,0,beta);
    }
}

Rotation::~Rotation() {}

void Rotation::_initRotationVectors()
{
    rotationVector[0]=_getRotationVectorX();
    rotationVector[1]=_getRotationVectorY(rotationVector[0],get(0));
    rotationVector[2]=_getRotationVectorZ(rotationVector[0],get(0),rotationVector[1],get(1));
}

Rotation Rotation::norm()const
{
    static struct rotation_unary_op:public unary_op<double,double>
    {
        virtual double operator()(const volatile double& t)const
        {
            return fmod(t,M_PI*2.0);
        }
    } _unary_op;

    return Rotation(Tuple<double,TREBLE_SIZE>(*this,_unary_op));
}

Rotation Rotation::reverse()const
{
    return Rotation(-(*this));
}

Rotation Rotation::random()
{
    static struct rotation_inplace_unary_op:public inplace_unary_op<double>
    {
        virtual void operator()(double &c)const
        {
            c=RND*2.0*M_PI;
        }
    } _rotation_inplace_unary_op;
    return Rotation(_rotation_inplace_unary_op);
}

Vector Rotation::_getRotationVectorX()
{
    return Vector::UnitX;
}

Vector Rotation::_getRotationVectorY(const Vector& vx,double rx)
{
    return Vector(Point(Vector::UnitY).axialRotate(vx,Mark::Ref.getOrig(),rx));
}

Vector Rotation::_getRotationVectorZ(const Vector& vx,double rx,const Vector& vy,double ry)
{
    return Vector(Point(Vector::UnitZ).axialRotate(vx,Mark::Ref.getOrig(),rx).axialRotate(vy,Mark::Ref.getOrig(),ry));
}

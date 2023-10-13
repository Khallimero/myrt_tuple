#include "Vector.h"
#include "Point.h"
#include "Mark.h"

#include <math.h>

Vector::Vector() {}

Vector::Vector(double x,double y,double z)
    :Geometric(Treble<double>(x,y,z)) {}

Vector::Vector(const Tuple<double,TREBLE_SIZE>& that)
    :Geometric(Treble<double>(that)) {}

Vector::~Vector() {}

double Vector::length()const
{
    return this->sqrt();
}

Vector Vector::norm()const
{
    if(isNull())return Vector::null;
    return (*this)/this->length();
}

Vector Vector::prodVect(const Vector& that)const
{
    struct vector_unary_idx_op:public unary_idx_op<double,TREBLE_SIZE>
    {
        vector_unary_idx_op(const Vector& v1,const Vector& v2)
            :_v1(v1),_v2(v2) {}
        virtual double operator()(const tuple_idx<TREBLE_SIZE>& i)const
        {
            int i1=i+1,i2=i+2;
            return _v1(i1)*_v2(i2)-_v1(i2)*_v2(i1);
        }
        const Vector& _v1,_v2;
    };

    return Vector(vector_unary_idx_op(*this,that));
}

double Vector::cosAngle(const Vector& that)const
{
    return this->prodScal(that)/(this->length()*that.length());
}

double Vector::angle(const Vector& that)const
{
    return acos(cosAngle(that));
}

Vector Vector::reflect(const Vector& normal)const
{
    Vector v1=this->norm();
    Vector v2=normal.norm();
    return v1-(v2*(2.0*v1.prodScal(v2)));
}

Vector Vector::refract(const Vector& normal,double n1,double n2)const
{
    Vector v1=this->norm();
    Vector v2=normal.norm();

    double r=n1/n2;
    double a1=v2.prodScal(Vector(-v1));
    double a2=1.0-SQ(r)*(1.0-SQ(a1));
    if(a2<0)return Vector::null;

    return v1*r+v2*(r*a1+::sqrt(a2)*(a1>=0.0?-1.0:1.0));
}

Vector Vector::getOrtho()const
{
    TUPLE_IDX(i,TREBLE_SIZE)
    if(!FZERO(this->get(i)))
    {
        Vector v=Vector::Unit;
        v[i]=-(this->get(i+1)+this->get(i+2))/this->get(i);
        return v.norm();
    }

    return Vector::null;
}

Vector Vector::projOrthoTo(const Vector& that)const
{
    return that.prodVect(this->prodVect(that));
}

Vector Vector::projOn(const Vector& that)const
{
    Vector v=that.norm();
    double f=this->prodScal(v);
    return v*f;
}

Vector Vector::random()
{
    static struct vector_unary_idx_op:public unary_idx_op<double,TREBLE_SIZE>
    {
        virtual double operator()(const tuple_idx<TREBLE_SIZE>&)const
        {
            return RND-0.5;
        }
    } _unary_idx_op;
    return Vector(_unary_idx_op).norm();
}

const Vector Vector::null=Vector(0,0,0);
const Vector Vector::UnitX=Vector(1,0,0);
const Vector Vector::UnitY=Vector(0,1,0);
const Vector Vector::UnitZ=Vector(0,0,1);
const Vector Vector::Unit=Vector::UnitX+Vector::UnitY+Vector::UnitZ;
const TupleRef<Vector> Vector::UnitTab[3]= {Vector::UnitX,Vector::UnitY,Vector::UnitZ};
const Vector Vector::EpsilonX=Vector::UnitX*EPSILON;
const Vector Vector::EpsilonY=Vector::UnitY*EPSILON;
const Vector Vector::EpsilonZ=Vector::UnitZ*EPSILON;
const TupleRef<Vector> Vector::EpsilonTab[3]= {Vector::EpsilonX,Vector::EpsilonY,Vector::EpsilonZ};

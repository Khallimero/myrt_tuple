#include "Point.h"

#include <math.h>

const Point Point::null=Point(0,0,0);

Point::Point() {}

Point::Point(double x,double y,double z):Geometric(Treble<double>(x,y,z)) {}

Point::Point(const Tuple<double,TREBLE_SIZE>& that):Geometric(that) {}

Point::~Point() {}

Vector Point::getVectorTo(const Point& that)const
{
    return Vector(that-(*this));
}

Point Point::centerRotate(const Point& c,const Rotation& r)const
{
    Point p=Point(*this);
    for(int i=0; i<3; i++)
        p=p.axialRotate(r.getRotationVector()[i],c,r.getRotationValue(i));

    return p;
}

Point Point::centerRotateInv(const Point& c,const Rotation& r)const
{
    Point p=Point(*this);
    Rotation rr=r.reverse();
    for(int i=0; i<3; i++)
        p=p.axialRotate(r.getRotationVector()[2-i],c,rr.getRotationValue(2-i));

    return p;
}

Point Point::axialRotate(const Vector& ax,const Point& o,double a)const
{
    if(FZERO(fmod(a,M_PI*2.0))||(*this==o))
        return Point(*this);

    struct point_unary_idx_op:public unary_idx_op<double,TREBLE_SIZE>
    {
        point_unary_idx_op(const Vector& ax,const Point& o,double a)
        {
            c=cos(a),s=sin(a);
            c1=1.0-c;
            u=ax.norm(),p=o;
        }
        virtual double operator()(const tuple_idx<TREBLE_SIZE>& i)const
        {
            int i1=i+1,i2=i+2;
            Point m=Point(SQ(u(i))+(1.0-SQ(u(i)))*c,
                          u(i)*u(i1)*c1-u(i2)*s,
                          u(i)*u(i2)*c1+u(i1)*s);
            return m.prodScal(p.rot(i));
        }

        double c,s,c1;
        Vector u;
        Point p;
    };

    return o+Point(point_unary_idx_op(ax,Point(*this)-o,a));
}

double Point::dist(const Point& that)const
{
    return getVectorTo(that).length();
}

Point Point::barycenter(const Point& p1,const Point& p2,
                        double a,double b)
{
    return Point::barycenter(p1,p2,Point::null,a,b,0);
}

Point Point::barycenter(const Point& p1,const Point& p2,const Point& p3,
                        double a,double b,double c)
{
    return Point((p1*a+p2*b+p3*c)/(a+b+c));
}

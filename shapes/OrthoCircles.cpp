#include "OrthoCircles.h"
#include "Sphere.h"
#include "SubShape.h"

#include <math.h>

OrthoCircles::OrthoCircles(double c1,double c2,
                           double size,
                           const Mark& mk)
    :IsoSurface(size,SmartPointer<const Shape>(new SubShape(new Sphere(1.2),new Sphere(.8))),mk)
{
    this->c1=c1,this->c2=c2;
}

OrthoCircles::~OrthoCircles() {}

double OrthoCircles::getValue(const Point& p)const
{
    struct point_tuple_idx_fct:public tuple_idx_fct<double,TREBLE_SIZE>
    {
        point_tuple_idx_fct(const Point& _p)
        {
            this->p=sq(_p);
            (**this)=1;
        }
        virtual void operator()(const tuple_idx<TREBLE_SIZE>& i)
        {
            int i1=i+1,i2=i+2;
            (**this)*=(SQ(p(i)+p(i1)-1)+p(i2));
        }
        const Point& getPoint()const
        {
            return p;
        }
        Point p;
    } _tuple_idx_fct(p);

    return _tuple_idx_fct.tamper()-SQ(c1)*(1+c2*sum(_tuple_idx_fct.getPoint()));
}

Vector OrthoCircles::getNormal(const Point& p)const
{
    struct point_unary_idx_op:public unary_idx_op<double,TREBLE_SIZE>
    {
        point_unary_idx_op(const Point& p,double c1,double c2)
        {
            static struct point_unary_op:public unary_op<double,double>
            {
                virtual double operator()(const volatile double& t)const
                {
                    return t-1;
                }
            } _unary_op;

            this->pp=sq(p);
            Tuple<double,TREBLE_SIZE> t1=this->pp+rot(this->pp);
            this->pp1=Point(Tuple<double,TREBLE_SIZE>(t1,_unary_op));
            Tuple<double,TREBLE_SIZE> t2=rot(sq(this->pp1))+this->pp;
            this->pps=t2*rot(t2)*2.0;
            this->c=2.0*SQ(c1)*c2;
            this->pp1*=2.0;
        }
        virtual double operator()(const tuple_idx<TREBLE_SIZE>& i)const
        {
            return sum(Vector(pp1(i),1,pp1(i+2))*rot(pps,i))-this->c;
        }
        Point pp,pp1,pps;
        double c;
    };

    return Vector(point_unary_idx_op(p,c1,c2))*p;
}
